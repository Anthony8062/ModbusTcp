#include "ClientModbusTcpRead.h"


//----------------------------------------------------------------------------------------------------
// Singleton
//----------------------------------------------------------------------------------------------------
CClientModbusTcpRead* CClientModbusTcpRead::m_pInstance = nullptr;
CClientModbusTcpRead* CClientModbusTcpRead::GetInstance()
{
	return m_pInstance ? m_pInstance : (m_pInstance = new CClientModbusTcpRead);
}

void CClientModbusTcpRead::DelInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


CClientModbusTcpRead::CClientModbusTcpRead(void)
{
	InitInfo();
}

CClientModbusTcpRead::~CClientModbusTcpRead(void)
{
	SocketCleanUp();
}

void CClientModbusTcpRead::InitInfo()
{
	memset(&sock_addr, 0, sizeof(sock_addr));
	memset(send_buff, 0, 8);
	memset(recv_buff, 0, 7);

	sock_addr.sin_family = AF_INET;                 //ipv4 protocol
	sock_addr.sin_addr.s_addr = inet_addr(NE_IP);   //htonl(INADDR_ANY); //서버의 주소 : 127.0.0.1 
	sock_addr.sin_port = htons(NE_PORT);            //htons(502);

	d_ID = 0xFF;                        // ID : 0xFF
	d_FC = READ_HOLDING_REGISTERS;      // RW : 0x03 -> READ_HOLDING_REGISTERS
	d_Addr = 6;
	d_Length = 1;

	timeout = { 2, 0 };
	FD_ZERO(&fdset);
	FD_SET(client_sock, &fdset);
}

bool CClientModbusTcpRead::SocketCleanUp()
{
	closesocket(client_sock);

	client_sock = INVALID_SOCKET;

	WSACleanup();
	return false;
}

bool CClientModbusTcpRead::Run_Socket()
{
	//-----------------------------------------------------------------------
	// WSAStartup함수는 프로세스에서 Winsock DLL의 사용을 초기화한다.
	//-----------------------------------------------------------------------
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	{
		printf("Device Read::WSAStartup() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// 소켓을 생성한다.
	//-----------------------------------------------------------------------
	if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Device Read::socket() Error\n");
		return SocketCleanUp();
	}

	printf("Modbus TCP for Device Read is working!\n");
	printf("Perform Client Connection!\n");


	//-----------------------------------------------------------------------
	// 접속을 시도 한다.
	//-----------------------------------------------------------------------	
	if (ConnectNonBlocking() != true)
	{
		printf("Device Read::ConnectNonBlock() Error\n");
		return SocketCleanUp();
	}
	
	
	//-----------------------------------------------------------------------
	// Connect 성공시 Slave에 Send, Recv 를 수행한다.
	//-----------------------------------------------------------------------
	if (ReadOrWrite() != true)
	{
		printf("Device Read::ReadOrWrite() Error\n");
		return SocketCleanUp();
	}

	SocketCleanUp();
	return true;
}

bool CClientModbusTcpRead::ConnectNonBlocking()
{
	int ret = 0;
	int err = 0;
	int len = 0;
	char error[255] = { 0, };

	if (connect(client_sock, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) < 0)
	{
		//printf("Device Read::connect() Error\n");
		//return SocketCleanUp();
		err = WSAGetLastError();
		if (err != EINPROGRESS && err != EWOULDBLOCK)
		{
			printf("Device Read::connect() err failed : %d\n", err);
			return SocketCleanUp();
		}

		if ((ret = select(client_sock + 1, &fdset, NULL, NULL, &timeout)) <= 0)
		{
			printf("Device Read::connect() select() Error\n");
			return SocketCleanUp();
		}

		if (FD_ISSET(client_sock, &fdset))
		{
			len = sizeof(error);
			if (getsockopt(client_sock, SOL_SOCKET, SO_ERROR, error, &len) < 0) // Solar is pending error
				printf("Device Read::connect() getsockopt() Error\n");
			return SocketCleanUp();
		}
		else
		{
			printf("select error: sockfd not set\n");
			return SocketCleanUp();
		}
	}

	return true;
}

bool CClientModbusTcpRead::ReadOrWrite()
{
	//-----------------------------------------------------------------------
	// send_buff[0] -> ID -> 0xff
	// send_buff[1] -> FC -> Function Code 0x03
	// send_buff[2] -> Addr -> 0x06 0x00 Address
	// send_buff[3] -> Addr
	// send_buff[4] -> Length -> 0x01 0x00 Length
	// send_buff[5] -> Length
	// send_buff[6] -> CRC
	// send_buff[7] -> CRC
	//-----------------------------------------------------------------------

	send_buff[0] = d_ID;
	send_buff[1] = d_FC;

	d_Addr = ReverseByteOrder(d_Addr);      // 0x06 0x00 -> 0x00 0x06
	memcpy(&send_buff[2], &d_Addr, 2);

	d_Length = ReverseByteOrder(d_Length);  // 0x01 0x00 -> 0x00 0x01  
	memcpy(&send_buff[4], &d_Length, 2);

	m_sCrc = CreateCRC16(send_buff, 6);     //54641 
	printf("Device Read - CRC::");
	PrintHexa((char*)&m_sCrc, 2);
	memcpy(&send_buff[6], &m_sCrc, 2);

	printf("Device Read - Send::");
	PrintHexa(send_buff, 8);

	if (send(client_sock, send_buff, 8, 0) == SOCKET_ERROR)
	{
		printf("Device Read::send() Error\n");
		return SocketCleanUp();
	}

	if ((recv_leng = recv(client_sock, recv_buff, 7, 0)) == SOCKET_ERROR)
	{
		printf("Device Read::recv() Error\n");
		return SocketCleanUp();
	}

	printf("Device Read - Recv::");
	PrintHexa(recv_buff, recv_leng);
	printf("Device Read ID : %02X\n", (unsigned char)recv_buff[4]);

	return true;
}

unsigned short CClientModbusTcpRead::ReverseByteOrder(unsigned short value)
{
	unsigned short ret = 0;

	((char*)&ret)[0] = ((char*)&value)[1];
	((char*)&ret)[1] = ((char*)&value)[0];

	return ret;
}

void CClientModbusTcpRead::PrintHexa(char* buff, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
	{
		printf("%02X ", (unsigned char)buff[i]);
	}
	printf("\n");
}

unsigned short CClientModbusTcpRead::CreateCRC16(char* buff, size_t len) // Checksum
{
	unsigned short crc16 = 0xFFFF;
	int i = 0;
	unsigned char* c_buff = (unsigned char*)buff;

	while (len--)
	{
		crc16 ^= *c_buff++;
		for (i = 0; i < 8; i++)
		{
			if (crc16 & 1)
			{
				crc16 >>= 1;
				crc16 ^= 0xA001;
			}
			else
			{
				crc16 >>= 1;
			}
		}
	}

	return crc16;
}
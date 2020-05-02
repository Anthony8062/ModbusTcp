#include "ClientModbusTcpWrite.h"


//----------------------------------------------------------------------------------------------------
// Singleton
//----------------------------------------------------------------------------------------------------
CClientModbusTcpWrite* CClientModbusTcpWrite::m_pInstance = nullptr;
CClientModbusTcpWrite* CClientModbusTcpWrite::GetInstance()
{
	return m_pInstance ? m_pInstance : (m_pInstance = new CClientModbusTcpWrite);
}

void CClientModbusTcpWrite::DelInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


CClientModbusTcpWrite::CClientModbusTcpWrite(void)
{
	InitInfo();
}

CClientModbusTcpWrite::~CClientModbusTcpWrite(void)
{
	SocketCleanUp();
}

void CClientModbusTcpWrite::InitInfo()
{
	memset(&sock_addr, 0, sizeof(sock_addr));
	memset(send_buff, 0, 8);
	memset(recv_buff, 0, 8);

	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = inet_addr(NE_IP);
	sock_addr.sin_port = htons(NE_PORT);

	d_ID = 0x02;                        //ID
	d_FC = WRITE_SINGLE_REGISTER;       //Write Single Register, 0x06
	d_Addr = 6;
	d_Value = 0x0001;   // Write Data

	timeout = { 2, 0 };
	FD_ZERO(&fdset);
	FD_SET(client_sock, &fdset);
}

bool CClientModbusTcpWrite::SocketCleanUp()
{
	closesocket(client_sock);

	client_sock = INVALID_SOCKET;

	WSACleanup();

	return false;
}

bool CClientModbusTcpWrite::Run_Socket()
{
	//-----------------------------------------------------------------------
	// WSAStartup함수는 프로세스에서 Winsock DLL의 사용을 초기화한다.
	//-----------------------------------------------------------------------
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	{
		printf("Device Write::WSAStartup() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// 소켓을 생성한다.
	//-----------------------------------------------------------------------
	if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Device Write::socket() Error\n");
		return SocketCleanUp();
	}

	printf("----------------------------------------\n");
	printf("Modbus TCP for Device Write is working!\n");
	printf("Perform Client Connection!\n");


	//-----------------------------------------------------------------------
	// 접속을 시도 한다.
	//-----------------------------------------------------------------------	
	if (ConnectNonBlocking() != true)
	{
		printf("Device Write::ConnectNonBlock() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// Connect 성공시 Slave에 Send, Recv 를 수행한다.
	//-----------------------------------------------------------------------
	if (ReadOrWrite() != true)
	{
		printf("Device Write::ReadOrWrite() Error\n");
		return SocketCleanUp();
	}

	SocketCleanUp();
	return true;
}

bool CClientModbusTcpWrite::ConnectNonBlocking()
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
			printf("Device Write::connect() err failed : %d\n", err);
			return SocketCleanUp();
		}

		if ((ret = select(client_sock + 1, &fdset, NULL, NULL, &timeout)) <= 0)
		{
			printf("Device Write::connect() select() Error\n");
			return SocketCleanUp();
		}

		if (FD_ISSET(client_sock, &fdset))
		{
			len = sizeof(error);
			if (getsockopt(client_sock, SOL_SOCKET, SO_ERROR, error, &len) < 0) // Solar is pending error
				printf("Device Write::connect() getsockopt() Error\n");
			return SocketCleanUp();
		}
		else
		{
			printf("Device Write::select error: sockfd not set\n");
			return SocketCleanUp();
		}
	}

	return true;
}

bool CClientModbusTcpWrite::ReadOrWrite()
{
	//-----------------------------------------------------------------------
	// send_buff[0] -> ID -> 0x02
	// send_buff[1] -> FC -> Function Code, 0x06 ,Write Single Register
	// send_buff[2] -> Addr -> 0x06 0x00 Address
	// send_buff[3] -> Addr
	// send_buff[4] -> Write Data -> 0x0001 Data, Register Value
	// send_buff[5] -> Write Data
	// send_buff[6] -> CRC
	// send_buff[7] -> CRC
	//-----------------------------------------------------------------------

	send_buff[0] = d_ID;
	send_buff[1] = d_FC;

	d_Addr = ReverseByteOrder(d_Addr);      // MSB -> LSB
	memcpy(&send_buff[2], &d_Addr, 2);

	d_Value = ReverseByteOrder(d_Value);    // MSB -> LSB 0x0001 -> 0x0100
	memcpy(&send_buff[4], &d_Value, 2);

	m_sCrc = CreateCRC16(send_buff, 6);     // Checksum
	printf("Device Write - CRC:: ");
	PrintHexa((char*)&m_sCrc, 2);
	memcpy(&send_buff[6], &m_sCrc, 2);


	printf("Device Write - Send::");
	PrintHexa(send_buff, 8);

	if (send(client_sock, send_buff, 8, 0) == SOCKET_ERROR)
	{
		printf("Device Write::send() Error\n");
		return SocketCleanUp();
	}

	if ((recv_leng = recv(client_sock, recv_buff, 8, 0)) == SOCKET_ERROR)
	{
		printf("Device Write::recv() Error\n");
		return SocketCleanUp();
	}

	printf("Device Write - Recv::");
	PrintHexa(recv_buff, recv_leng);

	return true;
}

unsigned short CClientModbusTcpWrite::ReverseByteOrder(unsigned short value)
{
	unsigned short ret = 0;

	((char*)&ret)[0] = ((char*)&value)[1];
	((char*)&ret)[1] = ((char*)&value)[0];

	return ret;
}

void CClientModbusTcpWrite::PrintHexa(char* buff, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
	{
		printf("%02X ", (unsigned char)buff[i]);
	}
	printf("\n");
}

unsigned short CClientModbusTcpWrite::CreateCRC16(char* buff, size_t len)
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
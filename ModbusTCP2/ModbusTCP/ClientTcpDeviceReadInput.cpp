#include "ClientTcpDeviceReadInput.h"


//----------------------------------------------------------------------------------------------------
// Singleton
//----------------------------------------------------------------------------------------------------
CClientTcpDeviceReadInput* CClientTcpDeviceReadInput::m_pInstance = nullptr;
CClientTcpDeviceReadInput* CClientTcpDeviceReadInput::GetInstance()
{
	return m_pInstance ? m_pInstance : (m_pInstance = new CClientTcpDeviceReadInput);
}

void CClientTcpDeviceReadInput::DelInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


CClientTcpDeviceReadInput::CClientTcpDeviceReadInput(void)
{
	InitInfo();
}

CClientTcpDeviceReadInput::~CClientTcpDeviceReadInput(void)
{
	SocketCleanUp();
}

void CClientTcpDeviceReadInput::InitInfo()
{
	memset(&sock_addr, 0, sizeof(sock_addr));
	memset(send_buff, 0, 8);
	memset(recv_buff, 0, 7);

	sock_addr.sin_family = AF_INET;                 //ipv4 protocol
	sock_addr.sin_addr.s_addr = inet_addr(NE_IP);   //htonl(INADDR_ANY); //서버의 주소 : 127.0.0.1 
	sock_addr.sin_port = htons(NE_PORT);            //htons(502);

	d_ID = 0xFD;                    // ID : 0xFD
	d_FC = READ_INPUT_REGISTER;     // FC : 0x04 -> READ_INPUT_REGISTER
	d_Addr = 6;
	d_Port = 1;
}

bool CClientTcpDeviceReadInput::SocketCleanUp()
{
	closesocket(client_sock);

	client_sock = INVALID_SOCKET;

	WSACleanup();

	return false;
}

bool CClientTcpDeviceReadInput::Run_Socket()
{
	//-----------------------------------------------------------------------
	// WSAStartup함수는 프로세스에서 Winsock DLL의 사용을 초기화한다.
	//-----------------------------------------------------------------------
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	{
		printf("Device Read Input::WSAStartup() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// 소켓을 생성한다.
	//-----------------------------------------------------------------------
	if ((client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Device Read Input::socket() Error\n");
		return SocketCleanUp();
	}

	printf("----------------------------------------\n");
	printf("Modbus TCP for Device Read Input is working!\n");
	printf("Perform Client Connection!\n");


	//-----------------------------------------------------------------------
	// 접속을 시도 한다.
	//-----------------------------------------------------------------------
	if (connect(client_sock, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
	{
		printf("Device Read Input::connect() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// Connect 성공시 Slave에 Send, Recv 를 수행한다.
	//-----------------------------------------------------------------------
	if (ReadOrWrite() != true)
	{
		printf("Device Read Input::ReadOrWrite() Error\n");
		return SocketCleanUp();
	}

	SocketCleanUp();
	return true;
}

bool CClientTcpDeviceReadInput::ReadOrWrite()
{
	//-----------------------------------------------------------------------          //-----------------------------------------------------------------------
	// send_buff[0] -> ID -> 0xFD                                                      // send_buff[0] -> ID -> 0x02
	// send_buff[1] -> FC -> Function Code 0x04                                        // send_buff[1] -> FC -> Function Code, 0x04
	// send_buff[2] -> Addr -> 0x06 0x00 Address                                       // send_buff[2] -> Addr -> 0x06 0x00 Address
	// send_buff[3] -> Addr                                                            // send_buff[3] -> Addr
	// send_buff[4] -> Port -> 0x01 0x00, Port Status                                  // send_buff[4] -> Write Data -> 0x0001 Data, Register Value
	// send_buff[5] -> Port                                                            // send_buff[5] -> Write Data
	// send_buff[6] -> CRC                                                             // send_buff[6] -> CRC
	// send_buff[7] -> CRC                                                             // send_buff[7] -> CRC
	//-----------------------------------------------------------------------          //-----------------------------------------------------------------------

	send_buff[0] = d_ID;
	send_buff[1] = d_FC;

	d_Addr = ReverseByteOrder(d_Addr);      // 0x06 0x00 -> 0x00 0x06
	memcpy(&send_buff[2], &d_Addr, 2);

	d_Port = ReverseByteOrder(d_Port);      // 0x01 0x00 -> 0x00 0x01
	memcpy(&send_buff[4], &d_Port, 2);

	m_sCrc = CreateCRC16(send_buff, 6);     //54641 
	printf("Device Read Input - CRC::");
	PrintHexa((char*)&m_sCrc, 2);
	memcpy(&send_buff[6], &m_sCrc, 2);

	printf("Device Read Input - Send::");
	PrintHexa(send_buff, 8);

	if (send(client_sock, send_buff, 8, 0) == SOCKET_ERROR)
	{
		printf("Device Read Input::send() Error\n");
		return SocketCleanUp();
	}

	if ((recv_leng = recv(client_sock, recv_buff, 7, 0)) == SOCKET_ERROR)
	{
		printf("Device Read Input::recv() Error\n");
		return SocketCleanUp();
	}

	printf("Device Read Input - Recv::");
	PrintHexa(recv_buff, recv_leng);
	printf("Device Read Input ID : %02X\n", (unsigned char)recv_buff[4]);

	return true;
}

unsigned short CClientTcpDeviceReadInput::ReverseByteOrder(unsigned short value)
{
	unsigned short ret = 0;

	((char*)&ret)[0] = ((char*)&value)[1];
	((char*)&ret)[1] = ((char*)&value)[0];

	return ret;
}

void CClientTcpDeviceReadInput::PrintHexa(char* buff, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
	{
		printf("%02X ", (unsigned char)buff[i]);
	}
	printf("\n");
}

unsigned short CClientTcpDeviceReadInput::CreateCRC16(char* buff, size_t len) // Checksum
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
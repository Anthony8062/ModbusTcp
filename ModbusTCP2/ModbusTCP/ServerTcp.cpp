#include "ServerTcp.h"
#include "ClientTcpDeviceRead.h"
#include "ClientTcpDeviceWrite.h"

//----------------------------------------------------------------------------------------------------
// Singleton
//----------------------------------------------------------------------------------------------------
CServerTcp* CServerTcp::m_pInstance = nullptr;
CServerTcp* CServerTcp::GetInstance()
{
	return m_pInstance ? m_pInstance : (m_pInstance = new CServerTcp);
}

void CServerTcp::DelInstance()
{
	if (m_pInstance)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

CServerTcp::CServerTcp(void)
{
	InitInfo();
}

CServerTcp::~CServerTcp(void)
{
	SocketCleanUp();
}

void CServerTcp::InitInfo()
{
	memset(reg_map, 0x00, sizeof(short) * 150);
	memset(&sock_addr, 0, sizeof(sock_addr));

	client_addr_size = sizeof(client_addr);
	memset(&client_addr, 0, client_addr_size);

	reg_map[6] = 0xFE;

	// 소켓 정보 - IP, port 정보를 기입한다.
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	sock_addr.sin_port = htons(SERVER_PORT);

	m_nConnectCount = 1;  // Default = 1
}

bool CServerTcp::SocketCleanUp()
{
	closesocket(local_sock);
	closesocket(client_sock);

	local_sock = INVALID_SOCKET;
	client_sock = INVALID_SOCKET;

	WSACleanup();

	return false;
}

bool CServerTcp::Run_Socket()
{
	if (GetCennectCount() < 1)
	{
		printf("Insufficient Connect Count");
		return false;
	}

	int nConnectCount = GetCennectCount();

	//-----------------------------------------------------------------------
	// WSAStartup함수는 프로세스에서 Winsock DLL의 사용을 초기화한다.
	//-----------------------------------------------------------------------
	if (WSAStartup(MAKEWORD(2, 2), &wsa) == SOCKET_ERROR)
	{
		printf("Server::WSAStartup() Error\n");
		return SocketCleanUp();
	}

	//-----------------------------------------------------------------------
	// 소켓을 생성한다.
	//-----------------------------------------------------------------------
	if ((local_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		printf("Server::socket() Error\n");
		return SocketCleanUp();
	}

	//-----------------------------------------------------------------------
	// 통신 관련 정보를 서버 소켓에 등록한다.
	//-----------------------------------------------------------------------
	if (bind(local_sock, (struct sockaddr*) & sock_addr, sizeof(sock_addr)) == SOCKET_ERROR)
	{
		printf("Server::bind() Error\n");
		return SocketCleanUp();
	}


	//-----------------------------------------------------------------------
	// 접속을 기다린다. Device Read Holding, Read Input, Device Write 등을 연결하기 위하여 접속 개수 n개로 설정한다. 
	//-----------------------------------------------------------------------
	if (listen(local_sock, nConnectCount) == SOCKET_ERROR)
	{
		printf("Server::listen() Error\n");
		return SocketCleanUp();
	}

	int i = 0;
	//for (i = 0; i < nConnectCount; i++)
	for(;;)
	{
		printf("-----------------------------------------------------------------------\n");
		printf("Modbus TCP for Server is working!\n");
		printf("Waiting Client Connection!\n");


		//-----------------------------------------------------------------------
		// Thread 를 돌려야 될거같고... 계속 대기중... 싱글턴을 해볼까?? fail -> Thread 가 하나라서... 결론은 Thread로 하자..
		//-----------------------------------------------------------------------
		if ((client_sock = accept(local_sock, (struct sockaddr*) & client_addr, &client_addr_size)) == SOCKET_ERROR)
		{
			printf("Server::accept() Error\n");
			return SocketCleanUp();
		}

		printf("Connected Client Count::%d\n", ++i);


		//-----------------------------------------------------------------------
		// Client로 부터 Accept 성공시 recv를 실행한다. 
		//-----------------------------------------------------------------------
		if (recv(client_sock, (char*)recv_buff, 8, 0) == SOCKET_ERROR)
		{
			printf("Server::recv() Error\n");
			return SocketCleanUp();
		}


		//-----------------------------------------------------------------------
		// Client로 부터 recv 성공시 Data Parsing 을 실행하여 Response 한다. 
		//-----------------------------------------------------------------------
		if (ReadOrWrite() != true)
		{
			printf("Server::ReadOrWrite() Error\n");
			return SocketCleanUp();
		}

		memset(recv_buff, 0, 8);
		closesocket(client_sock);
		client_sock = INVALID_SOCKET;
	}

	SocketCleanUp();
	system("pause");
	return true;
}

bool CServerTcp::ReadOrWrite()
{
	if ((recv_buff[0] == reg_map[6]) || (recv_buff[0] == 0xFF))
	{
		ReadHoldingRegister();
	}
	else if ((recv_buff[0] == reg_map[6]) || (recv_buff[0] == 0xFD))
	{
		ReadInputRegister();
	}
	else if ((recv_buff[0] == reg_map[6]) || (recv_buff[0] == 0x02))
	{
		WriteSingleRegister();
	}
	else if ((recv_buff[0] == reg_map[6]) || (recv_buff[0] == 0x03))
	{
		WriteMultipleRegister();
	}
	else
	{
		printf("Server to Device Write::ID Check Please!\n");
		return false;
	}

	return true;
}

bool CServerTcp::ReadHoldingRegister()
{
	// 0x03
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	// recv_buff[0] -> ID (0xFF) or 0xFE                                         // send_buff[0] -> ID (0xFF) or 0xFE 
	// recv_buff[1] -> FunctionCode                                              // send_buff[1] -> FunctionCode
	// recv_buff[2] -> StartingAddress addr                                      // send_buff[2] -> Length       2byte, 1 -> 2
	// recv_buff[3] -> StartingAddress addr                                      // send_buff[3] -> Output Data  2byte
	// recv_buff[4] -> Length                                                    // send_buff[4] -> 
	// recv_buff[5] -> Length                                                    // send_buff[5] -> CRC
	// recv_buff[6] -> CRC                                                       // send_buff[6] -> 
	// recv_buff[7] -> CRC                                                       // send_buff[7] -> 
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	
	if (recv_buff[1] == READ_HOLDING_REGISTERS) // 0x03 read Function Code
	{
		printf("Server - Recv::");
		PrintHexa(recv_buff, 8);

		memcpy(&m_sAddress, &recv_buff[2], 2);                      // [2][3] -> Starting Address                     
		m_sAddress = ReverseByteOrder(m_sAddress);                  // LSB -> MSB

		memcpy(&m_sLength, &recv_buff[4], 2);                       // [4][5] -> Length                                      
		m_sLength = ReverseByteOrder(m_sLength);                    // LSB -> MSB

		//send_buff = (unsigned char*)malloc(3 + m_sLength * 2 + 2);  // malloc 7 .. ID , r/w function code, length, register no to read, CRC2bytes
		send_buff[0] = recv_buff[0];
		send_buff[1] = READ_HOLDING_REGISTERS;
		send_buff[2] = (unsigned char)m_sLength * 2;                //2

		unsigned short i;
		for (i = 0; i < m_sLength; i++)
		{
			m_sValue = reg_map[m_sAddress + i];                     // Start Address ~ to Length
			m_sValue = ReverseByteOrder(m_sValue);                  // MSB -> LSB 보낼때는 LSB로 바꿔서
			memcpy(&send_buff[3 + i * 2], &m_sValue, 2);            // Copy to Send Buffer
		}

		m_sCrc = CreateCRC16(send_buff, 3 + m_sLength * 2);         // 7 Checksum
		memcpy(&send_buff[3 + m_sLength * 2], &m_sCrc, 2);

		if (send(client_sock, (char*)send_buff, 3 + m_sLength * 2 + 2, 0) == SOCKET_ERROR) // Response Sending
		{
			printf("Server to Device Read::send() Error\n");
			return SocketCleanUp();
		}

		printf("Server - Send::");
		PrintHexa(send_buff, 8);

		//free(send_buff);
		//send_buff = NULL;
	}

	return true;
}

bool CServerTcp::ReadInputRegister()
{
	//0x04
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	// recv_buff[0] -> ID (0xFD) or 0xFE                                         // send_buff[0] -> ID (0xFD) or 0xFE 
	// recv_buff[1] -> FunctionCode                                              // send_buff[1] -> FunctionCode
	// recv_buff[2] -> StartingAddress addr                                      // send_buff[2] -> Length       2byte
	// recv_buff[3] -> StartingAddress addr                                      // send_buff[3] -> Output Data  2byte
	// recv_buff[4] -> Port Status                                               // send_buff[4] -> 
	// recv_buff[5] -> Port Status                                               // send_buff[5] -> CRC
	// recv_buff[6] -> CRC                                                       // send_buff[6] -> 
	// recv_buff[7] -> CRC                                                       // send_buff[7] -> 
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	if (recv_buff[1] == READ_INPUT_REGISTER)
	{
		printf("Server - Recv::");
		PrintHexa(recv_buff, 8);

		memcpy(&m_sAddress, &recv_buff[2], 2);           // [2][3] -> Starting Address         
		m_sAddress = ReverseByteOrder(m_sAddress);       // LSB -> MSB

		memcpy(&m_sStatus, &recv_buff[4], 2);            // [4][5] -> Port Status 
		m_sStatus = ReverseByteOrder(m_sStatus);         // LSB -> MSB

		//send_buff = (unsigned char*)malloc(3 + m_sLength * 2 + 3);  // malloc 8
		send_buff[0] = recv_buff[0];
		send_buff[1] = READ_INPUT_REGISTER;
		send_buff[2] = (unsigned char)m_sLength * 2;                //2

		unsigned short i;
		for (i = 0; i < m_sLength; i++)
		{
			m_sStatus = reg_map[m_sAddress + i];                     // Start Address ~ to Length = Set Port
			m_sStatus = ReverseByteOrder(m_sStatus);   // MSB -> LSB 보낼때는 LSB로 바꿔서
			memcpy(&send_buff[3 + i * 2], &m_sStatus, 2);            // Copy to Send Buffer, Register Value, element 3 5 7 9
		}

		m_sCrc = CreateCRC16(send_buff, 3 + m_sLength * 2);         // 7 Checksum
		memcpy(&send_buff[3 + m_sLength * 2], &m_sCrc, 2);

		if (send(client_sock, (char*)send_buff, 3 + m_sLength * 2 + 2, 0) == SOCKET_ERROR) // Response Sending
		{
			printf("Server to Device Read Input::send() Error\n");
			return SocketCleanUp();
		}

		printf("Server - Send::");
		PrintHexa(send_buff, 8);

		//free(send_buff);
		//send_buff = NULL;
	}

	return true;
}

bool CServerTcp::WriteSingleRegister()
{
	//0x06
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	// recv_buff[0] -> ID (0x02) or 0xFE                                         // send_buff[0] -> ID (0x02) or 0xFE 
	// recv_buff[1] -> FunctionCode , 0x06 ,Write Single Register                // send_buff[1] -> FunctionCode
	// recv_buff[2] -> StartingAddress addr, 0x06                                // send_buff[2] -> Length      
	// recv_buff[3] -> StartingAddress addr                                      // send_buff[3] -> Register Value
	// recv_buff[4] -> Write Data -> 0x0001 Data                                 // send_buff[4] -> 
	// recv_buff[5] -> Write Data                                                // send_buff[5] -> CRC
	// recv_buff[6] -> CRC                                                       // send_buff[6] -> 
	// recv_buff[7] -> CRC                                                       // send_buff[7] -> 
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------

	if (recv_buff[1] == WRITE_SINGLE_REGISTER)
	{
		printf("Server - Recv::");
		PrintHexa(recv_buff, 8);

		memcpy(&m_sAddress, &recv_buff[2], 2);                      // [2][3] -> Starting Address   
		m_sAddress = ReverseByteOrder(m_sAddress);                  // LSB -> MSB

		memcpy(&m_sValue, &recv_buff[4], 2);                        // [4][5] -> Write Data              
		m_sValue = ReverseByteOrder(m_sValue);                      // LSB -> MSB

		//send_buff = (unsigned char*)malloc(3 + m_sLength * 2 + 3);  // malloc 8
		send_buff[0] = recv_buff[0];
		send_buff[1] = WRITE_SINGLE_REGISTER;
		send_buff[2] = (unsigned char)m_sLength * 2;                //2

		unsigned short i;
		for (i = 0; i < m_sLength; i++)
		{
			reg_map[m_sAddress + i] = m_sValue;                     // Start Address ~ to Length = Set Port
			m_rValue2 = ReverseByteOrder(reg_map[m_sAddress + i]);  // MSB -> LSB 보낼때는 LSB로 바꿔서
			memcpy(&send_buff[3 + i * 2], &m_rValue2, 2);           // Copy to Send Buffer, Register Value
		}

		m_sCrc = CreateCRC16(send_buff, 3 + m_sLength * 2);         // 7 Checksum
		memcpy(&send_buff[3 + m_sLength * 2], &m_sCrc, 2);

		if (send(client_sock, (char*)send_buff, 3 + m_sLength * 2 + 3, 0) == SOCKET_ERROR) // Response Sending
		{
			printf("Server to Device Write::send() Error\n");
			return SocketCleanUp();
		}

		printf("Server - Send::");
		PrintHexa(send_buff, 8);

		//free(send_buff);
		//send_buff = NULL;
	}

	return true;
}

bool CServerTcp::WriteMultipleRegister()
{
	//0x10
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------
	// recv_buff[0] -> ID (0x02) or 0xFE                                         // send_buff[0] -> ID (0x02) or 0xFE 
	// recv_buff[1] -> FunctionCode , 0x10 ,Write Multi Register                 // send_buff[1] -> FunctionCode
	// recv_buff[2] -> StartingAddress addr, 0x06                                // send_buff[2] -> Length      
	// recv_buff[3] -> StartingAddress addr                                      // send_buff[3] -> Register Value -> 0x0400
	// recv_buff[4] -> Write Data -> 0x0004 Data                                 // send_buff[4] -> 
	// recv_buff[5] -> Write Data                                                // send_buff[5] -> CRC
	// recv_buff[6] -> CRC                                                       // send_buff[6] -> 
	// recv_buff[7] -> CRC                                                       // send_buff[7] -> 
	//-----------------------------------------------------------------------    //-----------------------------------------------------------------------

	if (recv_buff[1] == WRITE_MULTIPLE_REGISTER)
	{
		printf("Server - Recv::");
		PrintHexa(recv_buff, 8);

		memcpy(&m_sAddress, &recv_buff[2], 2);                      // [2][3] -> Starting Address   
		m_sAddress = ReverseByteOrder(m_sAddress);                  // LSB -> MSB

		memcpy(&m_sValue, &recv_buff[4], 2);                        // [4][5] -> Write Data              
		m_sValue = ReverseByteOrder(m_sValue);                      // LSB -> MSB

		//send_buff = (unsigned char*)malloc(3 + m_sLength * 2 + 3);  // malloc 8
		send_buff[0] = recv_buff[0];
		send_buff[1] = WRITE_MULTIPLE_REGISTER;
		send_buff[2] = (unsigned char)m_sLength * 2;                //2

		unsigned short i;
		for (i = 0; i < m_sLength; i++)
		{
			reg_map[m_sAddress + i] = m_sValue;                     // Start Address ~ to Length = Set Port
			m_rValue2 = ReverseByteOrder(reg_map[m_sAddress + i]);  // MSB -> LSB 보낼때는 LSB로 바꿔서
			memcpy(&send_buff[3 + i * 2], &m_rValue2, 2);           // Copy to Send Buffer, Register Value
		}

		m_sCrc = CreateCRC16(send_buff, 3 + m_sLength * 2);         // 7 Checksum
		memcpy(&send_buff[3 + m_sLength * 2], &m_sCrc, 2);

		if (send(client_sock, (char*)send_buff, 3 + m_sLength * 2 + 3, 0) == SOCKET_ERROR) // Response Sending
		{
			printf("Server to Device Write Multiple::send() Error\n");
			return SocketCleanUp();
		}

		printf("Server - Send::");
		PrintHexa(send_buff, 8);

		//free(send_buff);
		//send_buff = NULL;
	}

	return true;
}

unsigned short CServerTcp::ReverseByteOrder(unsigned short value) //big endian <-> little endian 으로 바꾸고
{
	unsigned short ret = 0;

	((char*)&ret)[0] = ((char*)&value)[1];
	((char*)&ret)[1] = ((char*)&value)[0];

	return ret;
}

void CServerTcp::PrintHexa(unsigned char* buff, size_t len)
{
	size_t i;
	for (i = 0; i < len; i++)
	{
		printf("%02X ", (unsigned char)buff[i]);
	}
	printf("\n");
}

unsigned short CServerTcp::CreateCRC16(unsigned char* buff, size_t len)
{
	unsigned short crc16 = 0xFFFF;
	int i = 0;
	while (len--)
	{
		crc16 ^= *buff++;

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
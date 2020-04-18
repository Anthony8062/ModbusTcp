#pragma once

#include "Define.h"
#include "TcpIpInfo.h"

class CClientTcpDeviceReadInput : public  CTcpIpInfo
{
private:
	WSADATA wsa;
	SOCKET client_sock;
	char send_buff[8];
	char recv_buff[7];
	struct sockaddr_in sock_addr;
	int recv_leng;

	unsigned char   d_ID;
	unsigned char   d_FC;
	unsigned short  d_Addr;
	unsigned short  d_Port;
	unsigned short  m_sCrc;

public:
	void        InitInfo();
	bool        SocketCleanUp();
	bool            Run_Socket();
	bool            ReadOrWrite();
	unsigned short      ReverseByteOrder(unsigned short value);
	unsigned short      CreateCRC16(char* buff, size_t len);
	void                PrintHexa(char* buff, size_t len);

	//----------------------------------------------------------------------------------------------------
	// Singleton
	//----------------------------------------------------------------------------------------------------
private:
	static CClientTcpDeviceReadInput* m_pInstance;

public:
	static CClientTcpDeviceReadInput* GetInstance();
	static void						  DelInstance();
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------

public:
	CClientTcpDeviceReadInput(void);
	~CClientTcpDeviceReadInput(void);
};
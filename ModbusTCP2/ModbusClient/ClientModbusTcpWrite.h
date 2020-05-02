#pragma once

#include "ClientDefine.h"
#include "ClientTcpIpInfo.h"


class CClientModbusTcpWrite : public CClientTcpIpInfo
{
private:
	WSADATA		wsa;
	SOCKET		client_sock;
	char		send_buff[8];
	char		recv_buff[8];
	SOCKADDR_IN sock_addr;
	int			recv_leng;

	fd_set		fdset;
	timeval		timeout;

	unsigned char    d_ID;
	unsigned char    d_FC;
	unsigned short   d_Addr;
	unsigned short   d_Value;
	unsigned short   m_sCrc;

public:
	void        InitInfo();
	bool        SocketCleanUp();
	bool            Run_Socket();
	bool			ConnectNonBlocking();
	bool            ReadOrWrite();
	unsigned short      ReverseByteOrder(unsigned short value);
	void                PrintHexa(char* buff, size_t len);
	unsigned short      CreateCRC16(char* buff, size_t len);


	//----------------------------------------------------------------------------------------------------
	// Singleton
	//----------------------------------------------------------------------------------------------------
private:
	static CClientModbusTcpWrite *m_pInstance;

public:
	static CClientModbusTcpWrite *GetInstance();
	static void					  DelInstance();
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------

public:
	CClientModbusTcpWrite(void);
	~CClientModbusTcpWrite(void);
};
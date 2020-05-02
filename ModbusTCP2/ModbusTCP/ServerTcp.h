#pragma once

#include "define.h"
#include "TcpIpInfo.h"

class CServerTcp : public CTcpIpInfo
{
private:
	WSADATA             wsa;
	SOCKET              local_sock;
	SOCKET              client_sock;
	struct sockaddr_in    sock_addr;
	struct sockaddr_in    client_addr;
	int                   client_addr_size;

	unsigned short      reg_map[150];
	unsigned char       recv_buff[8];
	unsigned char       send_buff[255];
	unsigned short      m_sAddress;
	unsigned short      m_sValue;
	unsigned short      m_sStatus;
	unsigned short      m_sLength;
	unsigned short      m_sCrc;
	unsigned short      m_rValue2;   // To Master Send
	int             m_nConnectCount;

public:
	void    InitInfo();
	bool    SocketCleanUp();
	bool        Run_Socket();
	bool        ReadOrWrite();
	bool            ReadHoldingRegister();
	bool            ReadInputRegister();
	bool            WriteSingleRegister();
	bool            WriteMultipleRegister();
	unsigned short      ReverseByteOrder(unsigned short value);
	void				PrintHexa(unsigned char* buff, size_t len);
	unsigned short      CreateCRC16(unsigned char* buff, size_t len);

	void    SetCennectCount(int nVal) { m_nConnectCount = nVal; }
	int     GetCennectCount() { return m_nConnectCount; }

	//----------------------------------------------------------------------------------------------------
	// Singleton
	//----------------------------------------------------------------------------------------------------
private:
	static CServerTcp* m_pInstance;

public:
	static CServerTcp* GetInstance();
	static void                         DelInstance();
	//----------------------------------------------------------------------------------------------------
	//----------------------------------------------------------------------------------------------------

public:
	CServerTcp(void);
	~CServerTcp(void);
};
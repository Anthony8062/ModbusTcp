#pragma once

class CClientTcpIpInfo
{
public:
	virtual void        InitInfo();
	virtual bool        SocketCleanUp();
	virtual bool           Run_Socket();
	virtual bool		   ConnectNonBlocking();
	virtual bool           ReadOrWrite();
	virtual unsigned short      ReverseByteOrder(unsigned short value);
	virtual unsigned short      CreateCRC16(void* buff, size_t len);

public:
	CClientTcpIpInfo();
	virtual ~CClientTcpIpInfo();
};
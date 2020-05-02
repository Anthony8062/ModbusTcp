#pragma once

class CTcpIpInfo
{
public:
	virtual void        InitInfo();
	virtual bool        SocketCleanUp();
	virtual bool           Run_Socket();
	virtual bool           ReadOrWrite();
	virtual unsigned short      ReverseByteOrder(unsigned short value);
	virtual unsigned short      CreateCRC16(void* buff, size_t len);

public:
	CTcpIpInfo();
	virtual ~CTcpIpInfo();
};
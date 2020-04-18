#pragma once
#include <vector>

using namespace std;

class CTcpIpInfo;
class CServerTcp;
class CClientTcpDeviceRead;
class CClientTcpDeviceReadInput;
class CClientTcpDeviceWrite;
class CClientTcpDeviceWriteMultiple;
class CModbusTCP
{
public:
	CServerTcp                      *m_pServer;
	CClientTcpDeviceRead            *m_pClientRead;
	CClientTcpDeviceReadInput       *m_pClientReadInput;
	CClientTcpDeviceWrite           *m_pClientWrite;
	CClientTcpDeviceWriteMultiple   *m_pClientWriteMulti;

	vector<CTcpIpInfo*>        m_pTcpInfo;

public:
	void Run__Socket();

public:
	CModbusTCP(void);
	~CModbusTCP(void);
};

int main(void)
{
	CModbusTCP* m_pModbusTCP = new CModbusTCP;

	m_pModbusTCP->Run__Socket();

	delete m_pModbusTCP;
	m_pModbusTCP = nullptr;

	return 0;
}
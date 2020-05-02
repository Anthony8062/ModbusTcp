#pragma once
#include <vector>

using namespace std;

class CClientTcpIpInfo;
class CClientModbusTcpRead;
class CClientModbusTcpReadInput;
class CClientModbusTcpWrite;
class CClientModbusTcpWriteMulti;
class CClientModbusTcp
{
public:
	CClientModbusTcpRead            *m_pClientRead;
	CClientModbusTcpReadInput       *m_pClientReadInput;
	CClientModbusTcpWrite           *m_pClientWrite;
	CClientModbusTcpWriteMulti   *m_pClientWriteMulti;

	vector<CClientTcpIpInfo*>        m_pTcpInfo;

public:
	void Run__Socket();

public:
	CClientModbusTcp(void);
	~CClientModbusTcp(void);
};

int main(void)
{
	CClientModbusTcp* m_pModbusTCP = new CClientModbusTcp;

	m_pModbusTCP->Run__Socket();

	delete m_pModbusTCP;
	m_pModbusTCP = nullptr;

	getchar();
	return 0;
}
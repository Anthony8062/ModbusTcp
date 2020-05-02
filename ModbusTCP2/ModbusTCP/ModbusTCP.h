#pragma once
#include <vector>

using namespace std;

class CTcpIpInfo;
class CServerTcp;
class CModbusTCP
{
public:
	CServerTcp                 *m_pServer;
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

	getchar();
	return 0;
}
#include "ModbusTCP.h"
#include "ServerTcp.h"
#include <windows.h>
#include <thread>

CModbusTCP::CModbusTCP(void)
	: m_pServer(new CServerTcp)
{
	m_pTcpInfo.push_back(m_pServer);
}

CModbusTCP::~CModbusTCP(void)
{
	for (size_t i = 0; i < m_pTcpInfo.size(); ++i)
	{
		if (m_pTcpInfo[i])
		{
			delete m_pTcpInfo[i];
			m_pTcpInfo[i] = nullptr;
		}
	}
	m_pTcpInfo.clear();
}

void CModbusTCP::Run__Socket()
{
	//-----------------------------------------------------------------------
	// Device 통신할 개수를 Setting 한다.  
	//-----------------------------------------------------------------------
	//CServerTcp::GetInstance()->SetCennectCount(2);

	m_pServer->SetCennectCount(16);

	m_pTcpInfo[0]->Run_Socket();

	//CServerTcp::DelInstance();
}
#include "ModbusTCP.h"
#include "ClientTcpDeviceRead.h"
#include "ClientTcpDeviceReadInput.h"
#include "ClientTcpDeviceWrite.h"
#include "ClientTcpDeviceWriteMultiple.h"
#include "ServerTcp.h"
#include <windows.h>
#include <thread>

CModbusTCP::CModbusTCP(void)
	: m_pServer(new CServerTcp)
	, m_pClientRead(new CClientTcpDeviceRead)
	, m_pClientReadInput(new CClientTcpDeviceReadInput)
	, m_pClientWrite(new CClientTcpDeviceWrite)
	, m_pClientWriteMulti(new CClientTcpDeviceWriteMultiple)
{
	m_pTcpInfo.push_back(m_pServer);
	m_pTcpInfo.push_back(m_pClientRead);
	m_pTcpInfo.push_back(m_pClientReadInput);
	m_pTcpInfo.push_back(m_pClientWrite);
	m_pTcpInfo.push_back(m_pClientWriteMulti);
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

	m_pServer->SetCennectCount(4);


	//-----------------------------------------------------------------------
	// 통신을 위한 Thread 를 생성한다.
	//-----------------------------------------------------------------------
	thread m_ThreadServer([&]() {m_pTcpInfo[0]->Run_Socket(); });
	Sleep(500);

	thread m_ThreadServerRead([&]() {m_pTcpInfo[1]->Run_Socket(); });
	Sleep(500);

	thread m_ThreadDeviceReadInput([&]() {m_pTcpInfo[2]->Run_Socket(); });
	Sleep(500);

	thread m_ThreadDeviceWrite([&]() {m_pTcpInfo[3]->Run_Socket(); });
	Sleep(500);

	thread m_ThreadDeviceWriteMulti([&]() {m_pTcpInfo[4]->Run_Socket(); });


	//-----------------------------------------------------------------------
	// Client, Server 순으로 Release 한다. Server 는 가장 마지막에 소멸한다.
	//-----------------------------------------------------------------------
	m_ThreadServerRead.join();
	m_ThreadDeviceReadInput.join();
	m_ThreadDeviceWrite.join();
	m_ThreadDeviceWriteMulti.join();
	m_ThreadServer.join();

	m_ThreadServerRead.detach();
	m_ThreadDeviceReadInput.detach(); // 간헐적으로 error 발생하네... 타이밍이 안맞나..
	m_ThreadDeviceWrite.detach();
	m_ThreadDeviceWriteMulti.detach();
	m_ThreadServer.detach();

	//CServerTcp::DelInstance();
}
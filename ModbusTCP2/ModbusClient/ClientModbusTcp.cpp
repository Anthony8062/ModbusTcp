#include "ClientModbusTcpRead.h"
#include "ClientModbusTcpReadInput.h"
#include "ClientModbusTcpWrite.h"
#include "ClientModbusTcpWriteMulti.h"
#include "ClientModbusTcp.h"

#include <windows.h>
#include <thread>

CClientModbusTcp::CClientModbusTcp(void)
	: m_pClientRead(new CClientModbusTcpRead)
	, m_pClientReadInput(new CClientModbusTcpReadInput)
	, m_pClientWrite(new CClientModbusTcpWrite)
	, m_pClientWriteMulti(new CClientModbusTcpWriteMulti)
{
	m_pTcpInfo.push_back(m_pClientRead);
	m_pTcpInfo.push_back(m_pClientReadInput);
	m_pTcpInfo.push_back(m_pClientWrite);
	m_pTcpInfo.push_back(m_pClientWriteMulti);
}

CClientModbusTcp::~CClientModbusTcp(void)
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

void CClientModbusTcp::Run__Socket()
{
	//-----------------------------------------------------------------------
	// Device 통신할 개수를 Setting 한다.  
	//-----------------------------------------------------------------------
	//CServerTcp::GetInstance()->SetCennectCount(2);

	//m_pServer->SetCennectCount(4);
	
	printf("***********************************************************************\n");
	printf("1. Read\n");
	printf("2. Read Input\n");
	printf("3. Write\n");
	printf("4. Write Multi\n");
	printf("***********************************************************************\n");

	long long i;
	int nInput;
	for (i = 0; i < LLONG_MAX; i++)
	{
		printf("----------------------------------------\n");
		printf(">> Protocol Choose Kind of Number: ");
		scanf("%d", &nInput);
		printf(">> Protocol Count : %d \n", i + 1);

		switch (nInput)
		{
		case enumREAD_COILS:
		{
			m_pTcpInfo[0]->Run_Socket();
			break;
		}
		case enumREAD_DISCREATE_INPUTS:
		{
			m_pTcpInfo[1]->Run_Socket();
			break;
		}
		case enumWRITE_SINGLE_COIL:
		{
			m_pTcpInfo[2]->Run_Socket();
			break;
		}
		case enumWRITE_MULTIPLE_COILS:
		{
			m_pTcpInfo[3]->Run_Socket();
			break;
		}
		case enumDEVICE_EXIT:
		{
			printf(">> ModbusTcp Exit\n");
			return;
		}
		default:
			printf(">> Nonexistent Protocol\n");
			break;
		}
	}
	
	//CServerTcp::DelInstance();
}
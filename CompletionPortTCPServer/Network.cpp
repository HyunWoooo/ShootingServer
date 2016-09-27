#include "Network.h"



CNetwork::CNetwork()
{

}


CNetwork::~CNetwork()
{
	WSACleanup();
}

CNetwork::CNetwork(CHAR* argv[])
{
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0) return;
}


void CNetwork::Init()
{
	// 윈속 초기화
	if (WSAStartup(MAKEWORD(2, 2), &m_wsa) != 0) return ;

	m_listener.Init();
}


void CNetwork::Accept(SOCKET* client)
{
	m_listener.Listen(client, &hcp);
}


void CNetwork::CreateIOCP()
{
	hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL) return;
}

void CNetwork::CreateThreads()
{
	m_thread.Create_Thread_Pool(hcp);
}


void CNetwork::Process()
{
	SOCKET client;

	Init();
	CreateIOCP();
	CreateThreads();

	while (1)
	{
		Accept(&client);
		//m_thread.m_lsocketInfo.push_back(m_listener.temp);

		int roomnumber = 0;

		while (true)
		{
			if (m_thread.m_mmsocketInfo.count(roomnumber) < 2)
			{
				m_listener.temp->roomnumber = roomnumber;
				m_thread.m_mmsocketInfo.insert(make_pair(roomnumber, m_listener.temp));
				
				break;
			}
			else
			{
				roomnumber++;
			}
			
		}
	}
}

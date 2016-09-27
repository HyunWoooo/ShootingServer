#pragma once
#include "Error.h"
#include "Listener.h"
#include "Thread.h"


class CNetwork
{
private:
	WSADATA m_wsa;
	HANDLE hcp;

	CListener m_listener;
	CThread m_thread;
	

public:
	CNetwork();
	~CNetwork();
	CNetwork(CHAR* argv[]);

	void Init();
	void Process();
	void Accept(SOCKET* client);
	void CreateThreads();
	void CreateIOCP();
};



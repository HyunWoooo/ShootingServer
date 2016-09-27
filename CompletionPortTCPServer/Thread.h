#pragma once
#include "Error.h"
#include "Message.h"

class CThread
{
private:
	

public:
	CThread();
	~CThread();

	static list<SOCKETINFO*> m_lsocketInfo;

	static multimap<int, SOCKETINFO*> m_mmsocketInfo;

	void Create_Thread_Pool(HANDLE hcp);

	static DWORD WINAPI WorkerThread(LPVOID arg);
};


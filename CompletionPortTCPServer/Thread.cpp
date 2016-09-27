#include "Thread.h"



CThread::CThread()
{
}


CThread::~CThread()
{
}


void CThread::Create_Thread_Pool(HANDLE hcp)
{
	// CPU 개수 확인
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	// (CPU 개수 * 2)개의 작업자 스레드 생성
	HANDLE hThread;
	for (int i = 0; i<(int)si.dwNumberOfProcessors * 2; i++) {
		hThread = CreateThread(NULL, 0, WorkerThread, hcp, 0, NULL);
		if (hThread == NULL) return;
		CloseHandle(hThread);
	}
}

list<SOCKETINFO*> CThread::m_lsocketInfo;
multimap<int, SOCKETINFO*> CThread::m_mmsocketInfo;

DWORD WINAPI CThread::WorkerThread(LPVOID arg)
{
	int retval;

	HANDLE hcp = (HANDLE)arg;

	CMessage* m_Message = new CMessage();

	char* UTF8ToANSI(const char * pszCode);

	static bool p_ready[2];
	static bool all_ready;
	static bool room_in[2];
	static bool all_room_in;

	while (1)
	{
		DWORD cbTransferred;
		SOCKET client_sock;
		SOCKETINFO *ptr;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred,
			(LPDWORD)&client_sock, (LPOVERLAPPED *)&ptr, INFINITE);

		// 클라이언트 정보 얻기
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(ptr->sock, (SOCKADDR *)&clientaddr, &addrlen);
		// 비동기 입출력 결과 확인
		if (retval == 0 || cbTransferred == 0) {
				if (retval == 0) {
					DWORD temp1, temp2;
					WSAGetOverlappedResult(ptr->sock, &ptr->overlapped,
						&temp1, FALSE, &temp2);
					ERROR->err_display("WSAGetOverlappedResult()");
				}
				
				try
				{
					//m_lsocketInfo.remove(ptr);
					std::pair<std::map<int, SOCKETINFO*>::iterator, std::map<int, SOCKETINFO*>::iterator> iter_pair;
					iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);
					for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
					{
						if (ptr == iter->second)
							m_mmsocketInfo.erase(iter);
					}
					closesocket(ptr->sock);
					printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
						inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
				}
				catch (int exception) {
					printf("바보");
				}
				continue;
		}

		int sum;

		memcpy(&sum, ptr->buf, sizeof(sum));

		// 데이터 전송량 갱신
		if (ptr->recvbytes == 0) {
			ptr->recvbytes = cbTransferred;
			ptr->sendbytes = 0;
			// 받은 데이터 출력
			ptr->buf[ptr->recvbytes] = '\0';

			int num = 0;
			list<SOCKETINFO*>::iterator itor = m_lsocketInfo.begin();

			std::pair<std::map<int, SOCKETINFO*>::iterator, std::map<int, SOCKETINFO*>::iterator> iter_pair;

			switch (sum)
			{
			case MESSAGE:
				m_Message->SetMessage(UTF8ToANSI(&ptr->buf[4]));

				printf("[TCP/%s:%d] ", inet_ntoa(clientaddr.sin_addr),
					ntohs(clientaddr.sin_port));

				cout << m_Message->GetMessageA() << endl;

				break;
			case GAME_READY:
			{
				bool ready_temp;

				memcpy(&ready_temp, &ptr->buf[4], sizeof(ready_temp));

				iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);

				int readynum = 0;

				for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
				{
					SOCKETINFO* temp = iter->second;

					if (temp->sock == ptr->sock)
					{
						iter->second->ready = true;
					}

					if (temp->ready)
						readynum++;
				}

				if (readynum == 2)
				{
					cout << "Ready!" << endl;
					all_ready = true;
				}

				//for (itor; itor != m_lsocketInfo.end(); itor++)
				//{

				//	SOCKETINFO* temp = *itor;
				//	if (temp->sock == ptr->sock)
				//	{
				//		printf("%d p Ready", num+1);
				//		p_ready[num] = ready_temp;
				//	}

				//	num++;
				//	
				//}

				//if (p_ready[0] && p_ready[1])
				//{
				//	printf("[TCP/%s:%d] ", inet_ntoa(clientaddr.sin_addr),
				//		ntohs(clientaddr.sin_port));

				//	cout << "Ready!" << endl;
				//}
			}

				break;
			case GAME_START:
				break;

			case ROOM_IN:
			{
				bool room_temp;

				memcpy(&room_temp, &ptr->buf[4], sizeof(room_temp));

				iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);

				int roominnum = 0;

				for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
				{
					SOCKETINFO* temp = iter->second;

					if (temp->sock == ptr->sock)
					{
						iter->second->roomin = true;
					}

					if (temp->roomin)
						roominnum++;
				}

				if (roominnum == 2)
				{
					cout << "Room In!" << endl;
					all_room_in = true;
				}

				//for (itor; itor != m_lsocketInfo.end(); itor++)
				//{

				//	SOCKETINFO* temp = *itor;
				//	if (temp->sock == ptr->sock)
				//	{
				//		room_in[num] = room_temp;
				//		printf("%d p Room In", num + 1);
				//	}

				//	num++;
				//}

				//if (room_in[0] && room_in[1])
				//{
				//	printf("[TCP/%s:%d] ", inet_ntoa(clientaddr.sin_addr),
				//		ntohs(clientaddr.sin_port));

				//	cout << "Room In!" << endl;
				//}
			}
				break;

			case PLAYER_INFO:
				
				break;
			default:
				break;
			}
		}
		else {
			ptr->sendbytes += cbTransferred;
		}

		if (ptr->recvbytes > ptr->sendbytes) {
			// 데이터 보내기
			//if (p_ready[0] && p_ready[1] && sum == GAME_READY)
			//{
			//	p_ready[0] = false;
			//	p_ready[1] = false;
			//	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			//	ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			//	ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			//	int temp = GAME_START;
			//	int num = 0;

			//	ZeroMemory(&ptr->buf, sizeof(ptr->buf));
			//	memcpy(ptr->buf, &temp, sizeof(temp));

			//	list<SOCKETINFO*>::iterator itor = m_lsocketInfo.begin();
			//	for (itor; itor != m_lsocketInfo.end(); itor++)
			//	{
			//		SOCKETINFO* temp = *itor;
			//		num++; 
			//		
			//		memcpy(&ptr->buf[4], &num, sizeof(temp) + sizeof(num));

			//		temp->recvbytes = ptr->recvbytes;
			//		temp->sendbytes = ptr->sendbytes;
			//		ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
			//		temp->wsabuf.buf = ptr->wsabuf.buf;
			//		temp->wsabuf.len = ptr->wsabuf.len;

			//		DWORD sendbytes;
			//		retval = WSASend(temp->sock, &temp->wsabuf, 1,
			//			&sendbytes, 0, &temp->overlapped, NULL);

			//		if (retval == SOCKET_ERROR) {
			//			if (WSAGetLastError() != WSA_IO_PENDING) {
			//				ERROR->err_display("WSASend()");
			//			}
			//		}
			//	}
			//}
			if (all_ready && sum == GAME_READY)
			{
				all_ready = false;

				ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
				ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
				ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

				int temp = GAME_START;
				int num = 0;

				ZeroMemory(&ptr->buf, sizeof(ptr->buf));
				memcpy(ptr->buf, &temp, sizeof(temp));

				std::pair<std::map<int, SOCKETINFO*>::iterator, std::map<int, SOCKETINFO*>::iterator> iter_pair;
				iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);

				for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
				{
					SOCKETINFO* temp = iter->second;

					num++;

					memcpy(&ptr->buf[4], &num, sizeof(temp) + sizeof(num));

					temp->recvbytes = ptr->recvbytes;
					temp->sendbytes = ptr->sendbytes;
					ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
					temp->wsabuf.buf = ptr->wsabuf.buf;
					temp->wsabuf.len = ptr->wsabuf.len;

					DWORD sendbytes;
					retval = WSASend(temp->sock, &temp->wsabuf, 1,
						&sendbytes, 0, &temp->overlapped, NULL);

					if (retval == SOCKET_ERROR) {
						if (WSAGetLastError() != WSA_IO_PENDING) {
							ERROR->err_display("WSASend()");
						}
					}
				}
			}
			//else if(room_in[0] && room_in[1] && sum == ROOM_IN)
			//{
			//	room_in[0] = false;
			//	room_in[1] = false;
			//	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			//	ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
			//	ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

			//	int temp = All_ROOM_IN;

			//	ZeroMemory(&ptr->buf, sizeof(ptr->buf));
			//	memcpy(ptr->buf, &temp, sizeof(temp));

			//	list<SOCKETINFO*>::iterator itor = m_lsocketInfo.begin();
			//	for (itor; itor != m_lsocketInfo.end(); itor++)
			//	{
			//		SOCKETINFO* temp = *itor;

			//		temp->recvbytes = ptr->recvbytes;
			//		temp->sendbytes = ptr->sendbytes;
			//		ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
			//		temp->wsabuf.buf = ptr->wsabuf.buf;
			//		temp->wsabuf.len = ptr->wsabuf.len;

			//		DWORD sendbytes;
			//		retval = WSASend(temp->sock, &temp->wsabuf, 1,
			//			&sendbytes, 0, &temp->overlapped, NULL);

			//		if (retval == SOCKET_ERROR) {
			//			if (WSAGetLastError() != WSA_IO_PENDING) {
			//				ERROR->err_display("WSASend()");
			//			}
			//		}
			//	}
			//}

			else if (all_room_in && sum == ROOM_IN)
			{
				all_room_in = false;

				ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
				ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
				ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

				int temp = All_ROOM_IN;

				ZeroMemory(&ptr->buf, sizeof(ptr->buf));
				memcpy(ptr->buf, &temp, sizeof(temp));

				std::pair<std::map<int, SOCKETINFO*>::iterator, std::map<int, SOCKETINFO*>::iterator> iter_pair;
				iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);

				for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
				{
					SOCKETINFO* temp = iter->second;

					temp->recvbytes = ptr->recvbytes;
					temp->sendbytes = ptr->sendbytes;
					ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
					temp->wsabuf.buf = ptr->wsabuf.buf;
					temp->wsabuf.len = ptr->wsabuf.len;

					DWORD sendbytes;
					retval = WSASend(temp->sock, &temp->wsabuf, 1,
						&sendbytes, 0, &temp->overlapped, NULL);

					if (retval == SOCKET_ERROR) {
						if (WSAGetLastError() != WSA_IO_PENDING) {
							ERROR->err_display("WSASend()");
						}
					}
				}
			}

			else
			{
				ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
				ptr->wsabuf.buf = ptr->buf + ptr->sendbytes;
				ptr->wsabuf.len = ptr->recvbytes - ptr->sendbytes;

				std::pair<std::map<int, SOCKETINFO*>::iterator, std::map<int, SOCKETINFO*>::iterator> iter_pair;
				iter_pair = m_mmsocketInfo.equal_range(ptr->roomnumber);

				for (std::multimap<int, SOCKETINFO*>::iterator iter = iter_pair.first; iter != iter_pair.second; ++iter)
				{
					SOCKETINFO* temp = iter->second;

					temp->recvbytes = ptr->recvbytes;
					temp->sendbytes = ptr->sendbytes;
					ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
					temp->wsabuf.buf = ptr->wsabuf.buf;
					temp->wsabuf.len = ptr->wsabuf.len;

					DWORD sendbytes;
					retval = WSASend(temp->sock, &temp->wsabuf, 1,
						&sendbytes, 0, &temp->overlapped, NULL);

					if (retval == SOCKET_ERROR) {
						if (WSAGetLastError() != WSA_IO_PENDING) {
							ERROR->err_display("WSASend()");
						}
					}
				}

				//list<SOCKETINFO*>::iterator itor = m_lsocketInfo.begin();
				//for (itor; itor != m_lsocketInfo.end(); itor++)
				//{
				//	SOCKETINFO* temp = *itor;

				//	temp->recvbytes = ptr->recvbytes;
				//	temp->sendbytes = ptr->sendbytes;
				//	ZeroMemory(&temp->overlapped, sizeof(temp->overlapped));
				//	temp->wsabuf.buf = ptr->wsabuf.buf;
				//	temp->wsabuf.len = ptr->wsabuf.len;

				//	DWORD sendbytes;
				//	retval = WSASend(temp->sock, &temp->wsabuf, 1,
				//		&sendbytes, 0, &temp->overlapped, NULL);

				//	if (retval == SOCKET_ERROR) {
				//		if (WSAGetLastError() != WSA_IO_PENDING) {
				//			ERROR->err_display("WSASend()");
				//		}
				//	}
				//}

					//DWORD sendbytes;
					//retval = WSASend(ptr->sock, &ptr->wsabuf, 1,
					//	&sendbytes, 0, &ptr->overlapped, NULL);

					//if (retval == SOCKET_ERROR) {
					//	if (WSAGetLastError() != WSA_IO_PENDING) {
					//		ERROR->err_display("WSASend()");
					//	}
					//}
			}
			
			continue;
		}
		else {
			ptr->recvbytes = 0;
			
			// 데이터 받기
			ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
			ptr->wsabuf.buf = ptr->buf;
			ptr->wsabuf.len = BUFSIZE;

			DWORD recvbytes;
			DWORD flags = 0;
			retval = WSARecv(ptr->sock, &ptr->wsabuf, 1,
				&recvbytes, &flags, &ptr->overlapped, NULL);
			if (retval == SOCKET_ERROR) {
				if (WSAGetLastError() != WSA_IO_PENDING) {
					ERROR->err_display("WSARecv()");
				}
				continue;
			}
		}
	}
}

char* UTF8ToANSI(const char * pszCode)
{
	BSTR    bstrWide;
	char*   pszAnsi;
	int     nLength;

	// 길이를 먼저 알아냄
	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	//길이 만큼 메로리 할당
	bstrWide = SysAllocStringLen(NULL, nLength);
	//변환 수행
	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	
	pszAnsi = new char[nLength];
	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);
	
	return pszAnsi;
}
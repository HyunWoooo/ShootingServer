#include "Listener.h"



CListener::CListener()
{
}


CListener::~CListener()
{
}


void CListener::Init()
{
	int retval;

	// socket()
	m_listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_sock == INVALID_SOCKET) ERROR->err_quit("socket()");

	// bind()
	SOCKADDR_IN m_serveraddr;

	ZeroMemory(&m_serveraddr, sizeof(m_serveraddr));
	m_serveraddr.sin_family = AF_INET;
	m_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(m_listen_sock, (SOCKADDR *)&m_serveraddr, sizeof(m_serveraddr));
	if (retval == SOCKET_ERROR) ERROR->err_quit("bind()");

	// listen()
	retval = listen(m_listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) ERROR->err_quit("listen()");
}


void CListener::Listen(SOCKET* client, HANDLE* hcp)
{
	int retval;
	int	m_addrlen;
	SOCKADDR_IN clientaddr;
	DWORD recvbytes, flags;

	// accept()
	m_addrlen = sizeof(clientaddr);
	*client = accept(m_listen_sock, (SOCKADDR *)&clientaddr, &m_addrlen);
	if (*client == INVALID_SOCKET) {
		ERROR->err_display("accept()");
		return;
	}
	printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// 소켓과 입출력 완료 포트 연결
	CreateIoCompletionPort((HANDLE)*client, *hcp, *client, 0);

	// 소켓 정보 구조체 할당
	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL) return;
	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
	ptr->sock = *client;
	ptr->recvbytes = ptr->sendbytes = 0;
	ptr->wsabuf.buf = ptr->buf;
	ptr->wsabuf.len = BUFSIZE;

	temp = ptr;

	// 비동기 입출력 시작
	flags = 0;
	retval = WSARecv(*client, &ptr->wsabuf, 1, &recvbytes,
		&flags, &ptr->overlapped, NULL);
	if (retval == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			ERROR->err_display("WSARecv()");
		}
		return;
	}
}
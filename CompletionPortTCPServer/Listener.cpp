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
	printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	// ���ϰ� ����� �Ϸ� ��Ʈ ����
	CreateIoCompletionPort((HANDLE)*client, *hcp, *client, 0);

	// ���� ���� ����ü �Ҵ�
	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL) return;
	ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
	ptr->sock = *client;
	ptr->recvbytes = ptr->sendbytes = 0;
	ptr->wsabuf.buf = ptr->buf;
	ptr->wsabuf.len = BUFSIZE;

	temp = ptr;

	// �񵿱� ����� ����
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
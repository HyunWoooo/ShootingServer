#pragma once
#include "Error.h"

class CListener
{
private:
	SOCKET m_listen_sock;
	
public:
	CListener();
	~CListener();

	void Init();
	void Listen(SOCKET* client, HANDLE* hcp);
	SOCKETINFO *temp;
};


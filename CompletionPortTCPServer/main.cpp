#include "Network.h"

void main()
{
	CNetwork* sock = new CNetwork();

	sock->Process();

	delete sock;
}
#pragma once
#include "Error.h"
#include "Packet.h"

class CPacketResolver
{
private:

public:
	CPacketResolver();
	~CPacketResolver();

	void SetPacket(CPacket* packet);
	int Resolve(byte* data);
};


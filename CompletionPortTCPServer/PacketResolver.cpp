#include "PacketResolver.h"



CPacketResolver::CPacketResolver()
{
}


CPacketResolver::~CPacketResolver()
{
}

void CPacketResolver::SetPacket(CPacket* packet)
{

}


int CPacketResolver::Resolve(byte* data)
{
	int s1 = data[0] & 0xFF;
	int s2 = data[1] & 0xFF;
	int s3 = data[2] & 0xFF;
	int s4 = data[3] & 0xFF;

	return ((s1 << 24) + (s2 << 16) + (s3 << 8) + (s4 << 0));
}
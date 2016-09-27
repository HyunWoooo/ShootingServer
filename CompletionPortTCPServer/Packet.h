#pragma once
#include "Error.h"

class CPacket
{
private:
	byte packet[512];
	byte header[4];
	byte body[508];

public:
	CPacket();
	~CPacket();

	void SetPacket(byte* data);
};


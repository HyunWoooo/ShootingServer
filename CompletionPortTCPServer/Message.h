#pragma once
#include "Error.h"

#pragma pack(push, 1)
class CMessage
{
private:
	string data;
	
public:
	CMessage();
	~CMessage();

	void SetMessage(string msg);

	string GetMessageA();
};
#pragma pack(pop)

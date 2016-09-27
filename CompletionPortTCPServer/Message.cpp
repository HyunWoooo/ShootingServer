#include "Message.h"



CMessage::CMessage()
{
}


CMessage::~CMessage()
{
}



void CMessage::SetMessage(string msg)
{
	data = msg;
}

string CMessage::GetMessageA()
{
	return data;
}
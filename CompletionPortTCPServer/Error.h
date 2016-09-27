#pragma once

#include "global.h"

class CError
{
private:
	CError(void);
public:
	~CError(void);

	static CError* GetInstance();
public:
	void err_display(char* msg);
	void err_quit(char* msg);
};


#define ERROR ( CError::GetInstance() )
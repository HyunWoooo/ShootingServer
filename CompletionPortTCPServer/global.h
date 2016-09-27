#pragma once

#pragma comment(lib, "ws2_32")
#include <winsock2.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#include <vector>
#include <list>
#include <map>

#define SERVERPORT 9000
#define BUFSIZE    512

struct SOCKETINFO
{
	OVERLAPPED overlapped;
	SOCKET sock;
	char buf[BUFSIZE + 1];
	int recvbytes;
	int sendbytes;
	WSABUF wsabuf;
	int roomnumber;
	bool roomin;
	bool ready;
};

enum PROTOCOL
{
	MESSAGE = 0,

	GAME_READY,

	GAME_START,

	ROOM_IN,

	All_ROOM_IN,

	PLAYER_INFO,
};



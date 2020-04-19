#pragma once
#include <WinSock2.h>
#include <string>

using namespace std;
const static int MAX_CLIENT_NAME_LENGTH = 10;

struct ClientSocketInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
};

struct ClientInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
	string clientName = "DEFAULT";
	int roomNum = -1;

	ClientInfo() {};
	ClientInfo(SOCKET sock) : clientSock(sock) {};
	ClientInfo(SOCKET sock, string name) : clientSock(sock), clientName(name) {};

	void OutRoom();

	bool IsAcceptingRoomNum(int _roomNum);
};

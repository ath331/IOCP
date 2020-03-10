#pragma once
#include <WinSock2.h>

const static int MAX_CLIENT_NAME_LENGTH = 10;

struct ClientSocketInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
};

struct ClientInfo
{
	SOCKET clientSock;
	const char clientName[MAX_CLIENT_NAME_LENGTH] = "DEFAULT";
	int roomNum = -1;
	ClientInfo(SOCKET sock);
	void OutRoom(int _roomNum);

	bool IsAcceptingRoomNum(int _roomNum);

	const ClientInfo& operator=(ClientInfo clientInfo);
};

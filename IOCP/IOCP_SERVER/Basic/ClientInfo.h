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
	SOCKET clientSocket;
	const char clientName[MAX_CLIENT_NAME_LENGTH] = "DEFAULT";
	int roomNum = -1;
};
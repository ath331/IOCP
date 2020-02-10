#pragma once
#include <WinSock2.h>

struct ClientInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
};
#pragma once
#include <WinSock2.h>

struct ClientInfo
{
	SOCKET hClntSock;
	SOCKADDR_IN clntAdr;
};
#pragma once
#include <WinSock2.h>
#include <vector>
const static int MAX_CLIENT_NAME_LENGTH = 10;

using namespace std;

struct ClientSocketInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
};

struct ClientInfo
{
	SOCKET clientSock;
	const char clientName[MAX_CLIENT_NAME_LENGTH] = "DEFAULT";
	vector<int> rooomNum;

	ClientInfo& operator=(ClientInfo clientInfo)
	{
		clientSock = clientInfo.clientSock;
		memcpy((void*)clientName, clientInfo.clientName,sizeof(clientInfo.clientName));
		return *this;
	}
};

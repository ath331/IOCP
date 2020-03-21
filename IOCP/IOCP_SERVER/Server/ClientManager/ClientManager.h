#pragma once
#include "ClientInfo.h"
#include <vector>
#include <map>

using namespace std;

class TcpSession;
class ClientManager
{
public:
	void PushClientInfo(ClientInfo* clientInfo);
	void PopClientInfo(SOCKET sock);
	void CloseClient(SOCKET sock);
	ClientInfo* GetClientInfo(SOCKET sock);
	map<SOCKET, TcpSession*> clientSessionMap;

private:
	vector<ClientInfo*> _clientVec;

	struct SearchClient
	{
		SearchClient(SOCKET sock) : _sock(sock) {}

		bool operator()(ClientInfo* clientInfo)
		{
			return _sock == clientInfo->clientSock;
		}

	private:
		SOCKET _sock = NULL;
	}; 
};
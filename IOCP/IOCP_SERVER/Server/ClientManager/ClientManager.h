#pragma once
#include "ClientInfo.h"
#include "Lock.h"
#include <vector>
#include <map>

using namespace std;

class TcpSession;
class ClientManager
{
public:
	void PushClientInfo(ClientInfo* clientInfo);
	void PushClientInfo(SOCKET sock, string name);
	void PushClientInfo(SOCKET sock,TcpSession* session);

	void PopClientInfo(SOCKET sock);
	void CloseClient(SOCKET sock);
	ClientInfo* GetClientInfo(SOCKET sock);
	TcpSession* GetClientSession(SOCKET sock);

private:
	vector<ClientInfo*> _clientVec;
	map<SOCKET, TcpSession*> _clientSessionMap;
	Lock _clientMapLock;

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
#include "ClientManager.h"
#include "../IOCP_SERVER/Server/Session/TcpSession.h"
#include <algorithm>

void ClientManager::PushClientInfo(ClientInfo* clientInfo)
{
	_clientVec.push_back(clientInfo);
}

void ClientManager::PushClientInfo(SOCKET sock, string name)
{
	ClientInfo* clientInfo = new ClientInfo(sock);
	clientInfo->clientName = name;
	PushClientInfo(clientInfo);
}

void ClientManager::PushClientInfo(SOCKET sock, TcpSession* session)
{
	LockGuard clientMapLockGuard(_clientMapLock);

	_clientSessionMap.insert(make_pair(sock, session));
}



void ClientManager::CloseClient(SOCKET sock)
{
	shutdown(sock, SD_BOTH);
	closesocket(sock);

	LockGuard clientMapLockGuard(_clientMapLock);

	delete _clientSessionMap.find(sock)->second;
	_clientSessionMap.erase(sock);
}


void ClientManager::PopClientInfo(SOCKET sock)
{
	auto iter = find_if(_clientVec.begin(), _clientVec.end(), SearchClient(sock));

	closesocket(sock);
	delete (*iter);
	_clientVec.erase(iter);
}

ClientInfo* ClientManager::GetClientInfo(SOCKET sock)
{
	return *find_if(_clientVec.begin(), _clientVec.end(), SearchClient(sock));
}

TcpSession* ClientManager::GetClientSession(SOCKET sock)
{
	LockGuard clientMapLockGuard(_clientMapLock);

	return _clientSessionMap.find(sock)->second;
}

#include "ClientManager.h"
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


void ClientManager::CloseClient(SOCKET sock)
{
	shutdown(sock, SD_BOTH);
	closesocket(sock);
	clientSessionMap.erase(sock);
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
#include "ClientManager.h"
#include <algorithm>
#include <iostream>

void ClientManager::PushClientInfo(ClientInfo* clientInfo)
{
	_clientVec.push_back(clientInfo);
	cout << clientInfo->clientName << " connect!" << endl;
}

void ClientManager::PopClientInfo(SOCKET sock)
{
	auto iter = find_if(_clientVec.begin(), _clientVec.end(), SearchClient(sock));

	cout << (*iter)->clientName << " out.." << endl;
	closesocket(sock);
	delete (*iter);
	_clientVec.erase(iter);
}

ClientInfo* ClientManager::GetClientInfo(SOCKET sock)
{
	return *find_if(_clientVec.begin(), _clientVec.end(), SearchClient(sock));
}
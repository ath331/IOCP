#include "ClientManager.h"
#include <iostream>

void ClientManager::PushClientInfo(ClientInfo clientInfo)
{
	_clientVec.push_back(clientInfo);
	cout << clientInfo.clientName << " connect!" << endl;
}

void ClientManager::PopClientInfo(SOCKET sock)
{
	for (auto iter = _clientVec.begin(); iter != _clientVec.end(); iter++)
	{
		if (iter->clientSock == sock)
		{
			cout << iter->clientName << " out.." << endl;
			closesocket(sock);
			_clientVec.erase(iter);
			break;
		}
	}
}
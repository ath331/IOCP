#include <iostream>

#include "ClientManager.h"

void ClientManager::InputClientInfo(SOCKET clientSocket)
{
	ClientInfo clientInfo = { clientSocket };
	_clientVec.push_back(clientInfo);

	std::cout << "client connected..\n";
}

void ClientManager::OutClient(SOCKET clientSocket)
{
	for (auto iter = _clientVec.begin(); iter != _clientVec.end(); iter++)
	{
		if (clientSocket == iter->clientSocket)
		{
			closesocket(clientSocket);
			_clientVec.erase(iter);
			cout << "Client Out.." << endl;
		}
	}
}
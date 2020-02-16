#pragma once
#include <vector>
#include "ClientInfo.h"

using namespace std;

class ClientManager
{
public:
	void PushClientInfo(SOCKET clientSocket);
	void OutClient(SOCKET clientSocket);
private:
	vector<ClientInfo> _clientVec;
};


#pragma once
#include "ClientInfo.h"
#include <vector>

using namespace std;

class ClientManager
{
public:
	void PushClientInfo(ClientInfo clientInfo);
	void PopClientInfo(SOCKET sock);
private:
	vector<ClientInfo> _clientVec;
};


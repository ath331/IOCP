#pragma once
#include "ClientInfo.h"
#include <vector>

using namespace std;

class Room
{
public:
	Room(ClientInfo clientInfo, int maxClientCount = 2, bool privateRoom = false)
		: _maxClientCount(maxClientCount), _isPublicRoom(privateRoom)
	{
		clientInfoVec.push_back(clientInfo);
	}

	vector<ClientInfo> clientInfoVec;
	bool GetAccessInfoThisRoom();

private:
	int _maxClientCount;
	bool _isPublicRoom = true;
	const int _passWord = 0000;
};


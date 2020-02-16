#pragma once
#include "ClientInfo.h"

#include <iostream>
#include <vector>

using namespace std;

class Room
{
public:
	Room(const char* name, ClientInfo clientInfo, int maxClientCount = 2, bool privateRoom = false)
		: _roomName(name), _maxClientCount(maxClientCount), _isPublicRoom(privateRoom)
	{
		cout << clientInfoVec.size() << "번방 " << name << "생성" << endl;
		clientInfoVec.push_back(clientInfo);
	}

	vector<ClientInfo> clientInfoVec;
	bool GetAccessInfoThisRoom();

private:
	const char* _roomName;
	int _maxClientCount;
	bool _isPublicRoom = true;
	const int _passWord = 0000;
};
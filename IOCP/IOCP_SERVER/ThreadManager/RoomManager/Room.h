#pragma once
#include "ClientInfo.h"

#include <iostream>
#include <vector>

using namespace std;

class Room
{
public:
	Room(int roomNum,const char* name, ClientInfo clientInfo, int maxClientCount = 2, bool privateRoom = false)
		: _roomNum(roomNum), _roomName(name), _maxClientCount(maxClientCount), _isPublicRoom(privateRoom)
	{
		clientInfoVec.push_back(clientInfo);
	}

	vector<ClientInfo> clientInfoVec;
	bool GetAccessInfoThisRoom();
	const char* GetRoomName();
	int GetMaxClientCount();
	int GetRoomNum();
private:
	int _roomNum = 0;
	const char* _roomName;
	int _maxClientCount;
	bool _isPublicRoom = true;
	const int _passWord = 0000;
};
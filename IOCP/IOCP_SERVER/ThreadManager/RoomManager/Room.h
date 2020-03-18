#pragma once
#include "ClientInfo.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;

class Room
{
public:
	Room(int roomNum, string name, ClientInfo* clientInfo, int maxClientCount = 2, bool privateRoom = false)
		: _roomNum(roomNum), _roomName(name), _maxClientCount(maxClientCount), _isPublicRoom(privateRoom)
	{
		clientInfoVec.push_back(clientInfo);
	}
	vector<ClientInfo*> clientInfoVec;
	bool GetAccessInfoThisRoom();
	string GetRoomName();
	int GetMaxClientCount();
	int GetRoomNum();
	int OutClientInRoom(SOCKET clientSock, int roomNum);

	const Room& operator=(Room roomInfo)
	{
		_roomNum = roomInfo._roomNum;
		_maxClientCount = roomInfo._maxClientCount;
		_roomName = roomInfo._roomName;
		_isPublicRoom = roomInfo._isPublicRoom;
		return *this;
	}

private:
	int _roomNum = -1;
	string _roomName;
	int _maxClientCount;
	bool _isPublicRoom = true;
	const int _passWord = 0000;
};
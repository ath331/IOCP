#pragma once
#include "Room.h"
#include "ClientInfo.h"
#include <vector>

using namespace std;

class RoomManager
{
public:
	void MakeRoom(const char* roomName, ClientInfo clientInfo, int maxClientCount = 2, bool privateRoom = false);
	void EnterRoom(int roomNum, ClientInfo clientInfo);
	int GetRoomVecSize();
private:
	vector<Room> _roomVec;
};
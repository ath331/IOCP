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
	void OutClientInRoom(SOCKET clientSock);
	int GetRoomVecSize();
	Room GetRoomInfo();
	int GetRoomCount();
private:
	int _roomCount = 0;
	vector<Room> _roomVec;
};
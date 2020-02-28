#pragma once
#include "Room.h"
#include "ClientInfo.h"
#include <vector>
#include <string>

using namespace std;

class RoomManager
{
public:
	void MakeRoom(string roomName, ClientInfo* clientInfo, int maxClientCount = 2, bool privateRoom = false);
	void EnterRoom(int roomNum, ClientInfo* clientInfo);
	void OutClientInRoom(SOCKET clientSock, int roomNum);
	int GetRoomVecSize();
	Room GetRoomInfo(int roomNum);
	int GetRoomCount();
private:
	int _roomCount = 0;
	vector<Room> _roomVec;
};
#include "RoomManager.h"

void RoomManager::MakeRoom(ClientInfo clientInfo, int maxClientCount, bool privateRoom)
{
	Room room(clientInfo, maxClientCount, privateRoom);
	_roomVec.push_back(room);
}

void RoomManager::EnterRoom(int roomNum, ClientInfo clientInfo)
{
	if (_roomVec.empty())
		return;

	_roomVec[roomNum].clientInfoVec.push_back(clientInfo);
}

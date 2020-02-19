#include "RoomManager.h"

void RoomManager::MakeRoom(const char* roomName, ClientInfo clientInfo, int maxClientCount, bool privateRoom)
{
	Room room(_roomCount,roomName,clientInfo, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << _roomCount << "번방 " << roomName << "생성" << endl;
	_roomCount++;
}

void RoomManager::EnterRoom(int roomNum, ClientInfo clientInfo)
{
	if (_roomVec.empty())
		return;

	_roomVec[roomNum].clientInfoVec.push_back(clientInfo);
}

int RoomManager::GetRoomVecSize()
{
	return _roomVec.size();
}

Room RoomManager::GetRoomInfo()
{
	//TODO : 지금은 처음 방만 리턴한다
	return _roomVec[0];
}

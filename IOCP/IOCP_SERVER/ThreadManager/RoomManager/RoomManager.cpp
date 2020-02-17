#include "RoomManager.h"

void RoomManager::MakeRoom(const char* roomName, ClientInfo clientInfo, int maxClientCount, bool privateRoom)
{
	Room room(roomName,clientInfo, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << GetRoomVecSize() << "번방 " << roomName << "생성" << endl;

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


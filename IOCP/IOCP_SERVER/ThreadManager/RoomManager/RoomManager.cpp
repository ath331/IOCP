#include "RoomManager.h"

void RoomManager::MakeRoom(string roomName, ClientInfo* clientInfo, int maxClientCount, bool privateRoom)
{
	Room room(_roomCount, roomName, clientInfo, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << _roomCount << "번방 " << roomName << "생성" << endl;
	_roomCount++;
}

void RoomManager::EnterRoom(int roomNum, ClientInfo* clientInfo)
{
	if (_roomVec.empty())
		return;

	for (auto iter = _roomVec.begin(); iter != _roomVec.end(); iter++)
	{
		if (iter->GetRoomNum() == roomNum)
		{
			iter->clientInfoVec.push_back(clientInfo);
		}
	}
}

int RoomManager::GetRoomVecSize()
{
	return _roomVec.size();
}

Room RoomManager::GetRoomInfo(int roomNum)
{
	for (auto iter = _roomVec.begin(); iter != _roomVec.end(); iter++)
	{
		if (roomNum == iter->GetRoomNum())
		{
			return *iter;
		}
	}
	//std::cout << roomNum << " No Room!" << std::endl;
}

void RoomManager::OutClientInRoom(SOCKET clientSock, int roomNum)
{
	for (auto iter = _roomVec.begin(); iter != _roomVec.end(); iter++)
	{
		if (iter->GetRoomNum() == roomNum)
		{
			int temp = iter->OutClientInRoom(clientSock, roomNum); //temp = 방의 남은 인원
			if (temp <= 0)
			{
				_roomVec.erase(iter);
				break;
			}
		}
	}
}

int RoomManager::GetRoomCount()
{
	return _roomCount;
}

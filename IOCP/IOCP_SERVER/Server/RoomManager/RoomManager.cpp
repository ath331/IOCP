#include "RoomManager.h"
#include "ClientManager/ClientInfo.h"
#include <packet.h>
#include <algorithm>

void RoomManager::MakeRoom(string roomName, int maxClientCount, bool privateRoom)
{
	LockGuard _roomLockGuard(_roomLock);

	Room room(_roomCount, roomName, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << _roomCount << "번방 " << roomName << "생성" << endl;
	_roomCount++;
}

void RoomManager::EnterRoom(int roomNum, ClientInfo* clientInfo)
{
	LockGuard _roomLockGuard(_roomLock);

	if (roomNum < 0 )
		return;
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
	LockGuard _roomLockGuard(_roomLock);

	return static_cast<int>(_roomVec.size());
}

Room RoomManager::GetRoomInfoByRoomNum(int roomNum)
{
	LockGuard _roomLockGuard(_roomLock);

	auto iter = (find_if(_roomVec.begin(), _roomVec.end(), _SearchRoom(roomNum)));
	if (iter != _roomVec.end())
		return *iter;
	else
	{
		Room room;
		return room;
	}
}

Room RoomManager::GetRoomInfoByCountNum(int count)
{
	LockGuard _roomLockGuard(_roomLock);

	return _roomVec[count];
}


void RoomManager::OutClientInRoom(SOCKET clientSock, int roomNum)
{
	LockGuard _roomLockGuard(_roomLock);

	if (roomNum < 0)
		return;

	auto iter = find_if(_roomVec.begin(), _roomVec.end(), _SearchRoom(roomNum));
	if (iter != _roomVec.end())
	{
		int temp = iter->OutClientInRoom(clientSock, roomNum); //temp = 방의 남은 인원
		if (temp <= 0)
		{
			_roomVec.erase(iter);
		}
	}
}

int RoomManager::GetRoomCount()
{
	LockGuard _roomLockGuard(_roomLock);

	return _roomCount;
}

void RoomManager::SettingRoomList(RES_PacketRoomList& resPacketRoomList)
{
	LockGuard _roomLockGuard(_roomLock);

	resPacketRoomList.maxRoomCount = static_cast<int>(_roomVec.size());

	if (resPacketRoomList.maxRoomCount > MAX_ROOM_COUNT)
		resPacketRoomList.maxRoomCount = MAX_ROOM_COUNT;

	int roomCount = static_cast<int>(_roomVec.size());
	if (_roomVec.size() > MAX_ROOM_COUNT)
		roomCount = MAX_ROOM_COUNT;

	for (int i = 0; i < roomCount; i++)
	{
		resPacketRoomList.roomInfoList[i].roomNum = GetRoomInfoByCountNum(i).GetRoomNum();
		memcpy(&resPacketRoomList.roomInfoList[i].roomName, GetRoomInfoByCountNum(i).GetRoomName().c_str(), strlen(GetRoomInfoByCountNum(i).GetRoomName().c_str()));
		resPacketRoomList.roomInfoList[i].maxClientInRoom = GetRoomInfoByCountNum(i).GetMaxClientCount();
		resPacketRoomList.roomInfoList[i].curClientNum = static_cast<int>(GetRoomInfoByCountNum(i).clientInfoVec.size());
	}
}
#include "RoomManager.h"
#include <packet.h>
#include <algorithm>

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
	return static_cast<int>(_roomVec.size());
}

Room RoomManager::GetRoomInfoByRoomNum(int roomNum)
{
	return *(find_if(_roomVec.begin(), _roomVec.end(), _SearchRoom(roomNum)));
}

Room RoomManager::GetRoomInfoByCountNum(int count)
{
	return _roomVec[count];
}


bool RoomManager::OutClientInRoom(SOCKET clientSock, int roomNum)
{
	auto iter = find_if(_roomVec.begin(), _roomVec.end(), _SearchRoom(roomNum));
	int temp = iter->OutClientInRoom(clientSock, roomNum); //temp = 방의 남은 인원
	if (temp <= 0)
	{
		_roomVec.erase(iter);
		return FALSE;
	}
	return TRUE;
}

int RoomManager::GetRoomCount()
{
	return _roomCount;
}

void RoomManager::SettingRoomList(RES_PacketRoomList& resPacketRoomList)
{
	if (resPacketRoomList.maxRoomCount > MAX_ROOM_COUNT)
		resPacketRoomList.maxRoomCount = MAX_ROOM_COUNT;

	for (int i = 0; i < resPacketRoomList.maxRoomCount; i++)
	{
		resPacketRoomList.roomInfoList[i].roomNum = GetRoomInfoByCountNum(i).GetRoomNum();
		memcpy(&resPacketRoomList.roomInfoList[i].roomName, GetRoomInfoByCountNum(i).GetRoomName().c_str(), strlen(GetRoomInfoByCountNum(i).GetRoomName().c_str()));
		resPacketRoomList.roomInfoList[i].maxClientInRoom = GetRoomInfoByCountNum(i).GetMaxClientCount();
		resPacketRoomList.roomInfoList[i].curClientNum = static_cast<int>(GetRoomInfoByCountNum(i).clientInfoVec.size());
	}
}


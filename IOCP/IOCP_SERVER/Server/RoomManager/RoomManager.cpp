#include "RoomManager.h"
#include "ClientManager/ClientInfo.h"
#include <packet.h>
#include <algorithm>

void RoomManager::MakeRoom(string roomName, int maxClientCount, bool privateRoom)
{
	Room room(_roomCount, roomName, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << _roomCount << "번방 " << roomName << "생성" << endl;
	_roomCount++;
}

void RoomManager::EnterRoom(int roomNum, ClientInfo* clientInfo)
{
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
	return static_cast<int>(_roomVec.size());
}

Room RoomManager::GetRoomInfoByRoomNum(int roomNum)
{
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
	return _roomVec[count];
}


void RoomManager::OutClientInRoom(SOCKET clientSock, int roomNum)
{
	if (roomNum < 0)
		return;

	auto iter = find_if(_roomVec.begin(), _roomVec.end(), _SearchRoom(roomNum));
	if (iter != _roomVec.end())
	{
		int temp = iter->OutClientInRoom(clientSock, roomNum); //temp = 방의 남은 인원
		if (temp <= 0)
		{
			_roomVec.erase(iter);
			//return FALSE;
		}
		//return TRUE;
	}
	//return FALSE;
}

int RoomManager::GetRoomCount()
{
	return _roomCount;
}

void RoomManager::SettingRoomList(RES_PacketRoomList& resPacketRoomList)
{
	resPacketRoomList.maxRoomCount = static_cast<int>(_roomVec.size());

	if (resPacketRoomList.maxRoomCount > MAX_ROOM_COUNT)
		resPacketRoomList.maxRoomCount = MAX_ROOM_COUNT;

	for (int i = 0; i < _roomVec.size(); i++)
	{
		resPacketRoomList.roomInfoList[i].roomNum = GetRoomInfoByCountNum(i).GetRoomNum();
		memcpy(&resPacketRoomList.roomInfoList[i].roomName, GetRoomInfoByCountNum(i).GetRoomName().c_str(), strlen(GetRoomInfoByCountNum(i).GetRoomName().c_str()));
		resPacketRoomList.roomInfoList[i].maxClientInRoom = GetRoomInfoByCountNum(i).GetMaxClientCount();
		resPacketRoomList.roomInfoList[i].curClientNum = static_cast<int>(GetRoomInfoByCountNum(i).clientInfoVec.size());
	}
}
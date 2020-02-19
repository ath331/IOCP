#include "RoomManager.h"

void RoomManager::MakeRoom(const char* roomName, ClientInfo clientInfo, int maxClientCount, bool privateRoom)
{
	Room room(_roomCount,roomName,clientInfo, maxClientCount, privateRoom);
	_roomVec.push_back(room);
	cout << _roomCount << "���� " << roomName << "����" << endl;
	_roomCount++;
}

void RoomManager::EnterRoom(int roomNum, ClientInfo clientInfo)
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

Room RoomManager::GetRoomInfo()
{
	//TODO : ������ ó�� �游 �����Ѵ�
	return _roomVec[0];
}

void RoomManager::OutClientInRoom(SOCKET clientSock)
{
	for (auto iter = _roomVec.begin() ; iter != _roomVec.end() ; iter++)
	{
		int temp = iter->OutClientInRoom(clientSock); //temp = ���� ���� �ο�
		if (temp <= 0)
		{
			iter = _roomVec.erase(iter);
			break;
		}
	}
}

int RoomManager::GetRoomCount()
{
	return _roomCount;
}

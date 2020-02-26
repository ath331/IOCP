#include "Room.h"

bool Room::GetAccessInfoThisRoom()
{
	return _isPublicRoom;
}

string Room::GetRoomName()
{
	return _roomName;
}
int Room::GetMaxClientCount()
{
	return _maxClientCount;
}

int Room::GetRoomNum()
{
	return _roomNum;
}

int Room::OutClientInRoom(SOCKET clientSock,int roomNum)
{
	for (auto iter = clientInfoVec.begin(); iter != clientInfoVec.end(); iter++)
	{
		if (iter->clientSock == clientSock && iter->roomNum == roomNum)
		{
			//TODO : client가 방에 들어갈떄마다 몇번방에 있는지 정보를 clientInfo에 저장해야함
			iter = clientInfoVec.erase(iter);
			return clientInfoVec.size();
		}
	}
}

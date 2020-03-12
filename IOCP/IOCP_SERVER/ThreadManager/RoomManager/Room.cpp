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
		if ((*iter)->clientSock == clientSock && (*iter)->IsAcceptingRoomNum(roomNum))
		{
			iter = clientInfoVec.erase(iter);
			return clientInfoVec.size();
		}
	}
}

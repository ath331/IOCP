#include "Room.h"

bool Room::GetAccessInfoThisRoom()
{
	return _isPublicRoom;
}

const char* Room::GetRoomName()
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

int Room::OutClientInRoom(SOCKET clientSock)
{
	for (auto iter = clientInfoVec.begin(); iter != clientInfoVec.end(); iter++)
	{
		if (iter->clientSock == clientSock)
		{
			//TODO : 방번호까지 조회해서 그 방에서만 나가게 하기
			iter = clientInfoVec.erase(iter);
			return clientInfoVec.size();
		}
	}
}

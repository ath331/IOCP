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
			//TODO : ���ȣ���� ��ȸ�ؼ� �� �濡���� ������ �ϱ�
			iter = clientInfoVec.erase(iter);
			return clientInfoVec.size();
		}
	}
}

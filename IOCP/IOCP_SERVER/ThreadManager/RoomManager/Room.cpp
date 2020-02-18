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
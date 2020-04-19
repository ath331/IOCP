#include "ClientInfo.h"

void ClientInfo::OutRoom()
{
	roomNum = -1;
}

bool ClientInfo::IsAcceptingRoomNum(int _roomNum)
{
	if (_roomNum != -1)
		return TRUE;
	return FALSE;
}
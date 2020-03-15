#include "ClientInfo.h"

void ClientInfo::OutRoom(int _roomNum)
{
	roomNum = -1;
}

bool ClientInfo::IsAcceptingRoomNum(int _roomNum)
{
	if (_roomNum != -1)
		return TRUE;
	return FALSE;
}

const ClientInfo& ClientInfo::operator=(ClientInfo clientInfo)
{
	clientSock = clientInfo.clientSock;
	memcpy((void*)clientName, clientInfo.clientName, sizeof(clientInfo.clientName));
	roomNum = clientInfo.roomNum;
	return *this;
}
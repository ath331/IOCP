#pragma once
#include <iostream>
#include <WinSock2.h>
#include <vector>
#include <algorithm>

const static int MAX_CLIENT_NAME_LENGTH = 10;

using namespace std;

struct ClientSocketInfo
{
	SOCKET clientSock;
	SOCKADDR_IN clientAdr;
};

struct ClientInfo
{
	SOCKET clientSock;
	const char clientName[MAX_CLIENT_NAME_LENGTH] = "DEFAULT";
	//vector<int> roomNum;
	int roomNum = -1;
	ClientInfo(SOCKET sock)
	{
		clientSock = sock;
	}
	void OutRoom(int _roomNum)
	{
		//roomNum.erase(remove(roomNum.begin(), roomNum.end(), _roomNum), roomNum.end());
		roomNum = -1;
	}

	bool IsAcceptingRoomNum(int _roomNum)
	{
		/*int i = 0;
		for (auto iter = roomNum.begin(); iter != roomNum.end(); iter++)
		{
			if (roomNum[i] == _roomNum)
			{
				return TRUE;
			}
			i++;
		}*/
		if (_roomNum != -1)
			return TRUE;
		return FALSE;
	}

	ClientInfo& operator=(ClientInfo clientInfo)
	{
		clientSock = clientInfo.clientSock;
		memcpy((void*)clientName, clientInfo.clientName, sizeof(clientInfo.clientName));
		roomNum = clientInfo.roomNum;
		/*for (int i = 0; i < clientInfo.roomNum.size(); i++)
		{
			roomNum[i] = clientInfo.roomNum[i];
		}*/
		return *this;
	}
};

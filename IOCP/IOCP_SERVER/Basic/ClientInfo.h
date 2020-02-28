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
	vector<int> roomNum;
	void OutRoom(int _roomNum)
	{
		cout << *(roomNum.begin()) << endl;
		cout << *(roomNum.end()-1) << endl;
		auto iter = roomNum.erase(roomNum.begin());

		//roomNum.erase(remove(roomNum.begin(), roomNum.end(), _roomNum), roomNum.end());
		/*int i = 0;
		for (auto iter = roomNum.begin(); iter != roomNum.end();)
		{
			cout << roomNum[i] << endl;
			if (roomNum[i] == _roomNum)
			{
				iter = roomNum.erase(iter);
				return;
			}
			else
			{
				i++;
				iter++;
			}
		}*/
	}
	bool IsAcceptingRoomNum(int _roomNum)
	{
		int i = 0;
		for (auto iter = roomNum.begin(); iter != roomNum.end(); iter++)
		{
			if (roomNum[i] == _roomNum)
			{
				return TRUE;
			}
			i++;
		}
		return FALSE;
	}

	ClientInfo& operator=(ClientInfo clientInfo)
	{
		clientSock = clientInfo.clientSock;
		memcpy((void*)clientName, clientInfo.clientName, sizeof(clientInfo.clientName));
		return *this;
	}
};

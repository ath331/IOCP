#pragma once
#include "Room.h"
#include "../ClientManager/ClientInfo.h"
#include <vector>
#include <string>

using namespace std;

class RoomManager
{
public:
	void MakeRoom(string roomName, ClientInfo* clientInfo, int maxClientCount = 2, bool privateRoom = false);
	void EnterRoom(int roomNum, ClientInfo* clientInfo);
	//남은 인원이 0이 되어 방을 삭제하면 FALSE반환
	bool OutClientInRoom(SOCKET clientSock, int roomNum);
	int GetRoomVecSize();
	Room GetRoomInfoByRoomNum(int roomNum);
	Room GetRoomInfoByCountNum(int count); //count번쨰 방 객체를 반환
	int GetRoomCount();
private:
	int _roomCount = 0;
	vector<Room> _roomVec;

	struct _SearchRoom
	{
		_SearchRoom(int roomNum) : _roomNum(roomNum) {}

		bool operator()(Room room)
		{
			return room.GetRoomNum() == _roomNum;
		}
	private:
		int _roomNum = -1;
	};
};
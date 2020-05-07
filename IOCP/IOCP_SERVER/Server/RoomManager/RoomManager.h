#pragma once
#include "Room.h"
#include "Lock.h"

using namespace std;

struct RES_PacketRoomList;
struct  ClientInfo;
class RoomManager
{
public:
	void MakeRoom(string roomName, int maxClientCount = 2, bool privateRoom = false);
	void EnterRoom(int roomNum, ClientInfo* clientInfo);
	void OutClientInRoom(SOCKET clientSock, int roomNum);
	int GetRoomVecSize();
	Room GetRoomInfoByRoomNum(int roomNum);
	Room GetRoomInfoByCountNum(int count);
	int GetRoomCount();

	void SettingRoomList(RES_PacketRoomList& pck);
private:
	
	int _roomCount = 0;
	vector<Room> _roomVec;
	Lock _roomLock;

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
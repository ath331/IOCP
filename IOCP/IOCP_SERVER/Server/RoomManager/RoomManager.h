#pragma once
#include "Room.h"

using namespace std;

struct RES_PacketRoomList;
struct  ClientInfo;
class RoomManager
{
public:
	//void MakeRoom(string roomName, ClientInfo* clientInfo, int maxClientCount = 2, bool privateRoom = false);
	void MakeRoom(string roomName, /*ClientInfo* clientInfo,*/ int maxClientCount = 2, bool privateRoom = false);
	void EnterRoom(int roomNum, ClientInfo* clientInfo);
	//���� �ο��� 0�� �Ǿ� ���� �����ϸ� FALSE��ȯ
	bool OutClientInRoom(SOCKET clientSock, int roomNum);
	int GetRoomVecSize();
	Room GetRoomInfoByRoomNum(int roomNum);
	Room GetRoomInfoByCountNum(int count); //count���� �� ��ü�� ��ȯ
	int GetRoomCount();

	void SettingRoomList(RES_PacketRoomList& pck);
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
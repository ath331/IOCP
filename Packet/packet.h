#pragma once
//server <-> client 통신관련 packet 저장
//RES_ is Server Packet
#include <WinSock2.h>
const static int MAX_NAME_LENGTH = 10;
const static int MAX_ROOM_NAME_LENGTH = 20;
const static int MAX_ROOM_COUNT = 20;

enum class PacketIndex : int
{
	Login,

	MAKE_ROOM,
	RES_MAKE_ROOM,

	ROOM_LIST,
	RES_ROOM_LIST,

	ENTER_ROOM,
	CLOSE_ROOM,

	SEND_MESSAGE, //Client의 채팅을 담은 패킷
};

struct PacketInfo
{
	SOCKET sock;
	PacketIndex packetIndex;
	const char* packetBuffer;
};

struct PacketHeader
{
	PacketIndex index = (PacketIndex)-1;
	int headerSize = sizeof(PacketHeader);
};

struct PacketLogin
{
	PacketHeader header;
	const char name[MAX_NAME_LENGTH] = {};
	//const char* password[]; //TOCO : DB추가시 아이디생성 로직 만들때 구현하기

	PacketLogin()
	{
		header.index = PacketIndex::Login;
		header.headerSize = sizeof(PacketLogin);
	}
};

struct PacketMakeRoom
{
	PacketHeader header;
	char roomName[MAX_ROOM_NAME_LENGTH] = {};
	int maxClientCount = 2;
	bool isPrivateRoom = false;
	int password = 0000;

	PacketMakeRoom()
	{
		header.index = PacketIndex::MAKE_ROOM;
		header.headerSize = sizeof(PacketMakeRoom);
	}
};

struct RES_PacketMakeRoom
{
	PacketHeader header;
	int roomNum = 100;

	RES_PacketMakeRoom()
	{
		header.index = PacketIndex::RES_MAKE_ROOM;
		header.headerSize = sizeof(RES_PacketMakeRoom);
	}
};

struct PacketRoomList
{
	PacketHeader header;

	PacketRoomList()
	{
		header.index = PacketIndex::ROOM_LIST;
		header.headerSize = sizeof(PacketRoomList);
	}
};

struct RES_PacketRoomList
{
	PacketHeader header;
	int maxRoomCount = 0;

	struct RoomInfo
	{
		int roomNum = -1;
		const char roomName[MAX_ROOM_NAME_LENGTH] = "";
		int maxClientInRoom = 10;
		int curClientNum = 1;
	};
	RoomInfo roomInfoList[MAX_ROOM_COUNT];
	RES_PacketRoomList()
	{
		header.index = PacketIndex::RES_ROOM_LIST;
		header.headerSize = sizeof(RES_PacketRoomList);
	}
};

struct PacketEnterRoom
{
	PacketHeader header;
	int roomNum = -1;

	PacketEnterRoom()
	{
		header.index = PacketIndex::ENTER_ROOM;
		header.headerSize = sizeof(PacketEnterRoom);
	}
};

struct PacketCloseRoom
{
	PacketHeader header;

	PacketCloseRoom()
	{
		header.index = PacketIndex::CLOSE_ROOM;
		header.headerSize = sizeof(PacketCloseRoom);
	}
};

struct PacketSendMessage
{
	PacketHeader header;
	const char buffer[500] = " ";
	int roomNum = -1;

	PacketSendMessage()
	{
		header.index = PacketIndex::SEND_MESSAGE;
		header.headerSize = sizeof(PacketSendMessage);
	}
};
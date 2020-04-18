#pragma once
//server <-> client 통신관련 packet 저장
//RES_ is Server Packet
#include <WinSock2.h>

const static int MAX_NAME_LENGTH = 10;
const static int MAX_ROOM_NAME_LENGTH = 20;
const static int MAX_ROOM_COUNT = 20;

using Pck = WSABUF;

enum class PacketIndex : int
{
	MAKE_ROOM,
	RES_MAKE_ROOM,

	ROOM_LIST,
	RES_ROOM_LIST,

	ENTER_ROOM,
	CLOSE_ROOM,

	SEND_MESSAGE, //Client의 채팅을 담은 패킷

	DB_INDEX, //패킷인덱스가 DB관련인지 소스에서 쉽게 접근하기위한 접근 INDEX
	Login,
	MAKE_CLIENT_ID_INFO,
	DB_INSERT_DATA,
	DB_UPDATE_NAME,
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

	virtual ~PacketHeader() {};
};

struct PacketLogin : public PacketHeader
{
	const char id[20] = "DEFUALT_ID";
	const char pw[20] = "DEFUALT_PW";
	const char name[MAX_NAME_LENGTH] = "DEFUALT";
	bool isSuccessIdCheck = FALSE;

	PacketLogin()
	{
		index = PacketIndex::Login;
		headerSize = sizeof(PacketLogin);
	}
};

struct PacketMakeRoom : public PacketHeader
{
	char roomName[MAX_ROOM_NAME_LENGTH] = {};
	int maxClientCount = 2;
	bool isPrivateRoom = false;
	int password = 0000;

	PacketMakeRoom()
	{
		index = PacketIndex::MAKE_ROOM;
		headerSize = sizeof(PacketMakeRoom);
	}
};

struct RES_PacketMakeRoom : public PacketHeader
{
	int roomNum = 100;

	RES_PacketMakeRoom()
	{
		index = PacketIndex::RES_MAKE_ROOM;
		headerSize = sizeof(RES_PacketMakeRoom);
	}
};

struct PacketRoomList : public PacketHeader
{
	PacketRoomList()
	{
		index = PacketIndex::ROOM_LIST;
		headerSize = sizeof(PacketRoomList);
	}
};

struct RES_PacketRoomList : public PacketHeader
{
	int maxRoomCount = 0;

	struct RoomInfo
	{
		int roomNum = -1;
		char roomName[MAX_ROOM_NAME_LENGTH] = "";
		int maxClientInRoom = 10;
		int curClientNum = 1;
	};
	RoomInfo roomInfoList[MAX_ROOM_COUNT];
	RES_PacketRoomList()
	{
		index = PacketIndex::RES_ROOM_LIST;
		headerSize = sizeof(RES_PacketRoomList);
	}
};

struct PacketEnterRoom : public PacketHeader
{
	int roomNum = -1;

	PacketEnterRoom()
	{
		index = PacketIndex::ENTER_ROOM;
		headerSize = sizeof(PacketEnterRoom);
	}
};

struct PacketCloseRoom : public PacketHeader
{
	int roomNum = -1;

	PacketCloseRoom()
	{
		index = PacketIndex::CLOSE_ROOM;
		headerSize = sizeof(PacketCloseRoom);
	}
};

struct PacketSendMessage : public PacketHeader
{
	const char buffer[500] = " ";
	int roomNum = -1;

	PacketSendMessage()
	{
		index = PacketIndex::SEND_MESSAGE;
		headerSize = sizeof(PacketSendMessage);
	}
};

struct PacketClientIdInfo : public PacketHeader
{
	const char id[20] = "";
	const char pw[20] = "";
	const char name[MAX_NAME_LENGTH] = "";
	bool isMakeId = FALSE;
	bool isChangeName = FALSE;

	PacketClientIdInfo()
	{
		index = PacketIndex::MAKE_CLIENT_ID_INFO;
		headerSize = sizeof(PacketClientIdInfo);
	}
};

struct PacketDBInsertData : public PacketHeader
{
	bool isSuccessInsertData = FALSE;

	PacketDBInsertData()
	{
		index = PacketIndex::DB_INSERT_DATA;
		headerSize = sizeof(PacketDBInsertData);
	}
};

//struct PacketDBUpdateName
//{
//	PacketHeader header;
//	const char name[MAX_NAME_LENGTH] = "";
//
//	PacketDBUpdateName()
//	{
//		header.index = PacketIndex::DB_UPDATE_NAME;
//		header.headerSize = sizeof(PacketDBUpdateName);
//	}
//};
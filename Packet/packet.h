#pragma once
//server <-> client ��Ű��� packet ����
//RES_ is Server Packet
#include <WinSock2.h>

const static int MAX_NAME_LENGTH = 10;
const static int MAX_ROOM_NAME_LENGTH = 20;

enum class PacketIndex : int
{
	Login,
	MAKE_ROOM,
	RES_MAKE_ROOM,
	ROOM_LIST,
	RES_ROOM_LIST,
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
	//const char* password[]; //TOCO : DB�߰��� ���̵���� ���� ���鶧 �����ϱ�

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
	//TODO : ListBox�� ����ϱ� ���� �ʿ��� ���� �������� �迭�� ����
	int maxRoomCount = 0;
	int roomNum = 0;
	const char roomName[MAX_ROOM_NAME_LENGTH] = "DEFAULT";
	int maxClientInRoom = 10;
	int curClientNum = 1;

	RES_PacketRoomList()
	{
		header.index = PacketIndex::RES_ROOM_LIST;
		header.headerSize = sizeof(RES_PacketRoomList);
	}
};
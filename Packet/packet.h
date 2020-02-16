#pragma once
//server <-> client ��Ű��� packet ����
#include <WinSock2.h>

const static int MAX_NAME_LENGTH = 10;
const static int MAX_ROOM_NAME_LENGTH = 20;

enum class PacketIndex : int
{
	Login,
	MAKE_ROOM,
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
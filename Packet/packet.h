#pragma once
//server <-> client ��Ű��� packet ����

const static int MAX_NAME_LENGTH = 10;

enum class PacketIndex : int
{
	Exit,
	Login,
};

struct PacketHeader
{
	PacketIndex index;
	int headerSize = sizeof(PacketHeader);
};

struct PacketLogin
{
	PacketHeader header;
	const char name[MAX_NAME_LENGTH] = {};
	//const char* password[];

	PacketLogin()
	{
		header.index = PacketIndex::Login;
		header.headerSize = sizeof(PacketLogin);
	}
};
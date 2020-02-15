#pragma once
//server <-> client 통신관련 packet 저장

const static int MAX_NAME_LENGTH = 10;

enum class PacketIndex : int
{
	Login,
};

struct PacketInfo
{
	PacketIndex packetIndex;
	const char* packetBuffer;
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
	//const char* password[]; //TOCO : DB추가시 아이디생성 로직 만들때 구현하기

	PacketLogin()
	{
		header.index = PacketIndex::Login;
		header.headerSize = sizeof(PacketLogin);
	}
};
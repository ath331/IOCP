#include <stdio.h>
#include <string>
#include <iostream>
#include "ClientLogic.h"
#include "packet.h"
#pragma warning(disable:4996)

void ErrorHandling(const char* msg);

void ClientLogic::Init(std::string IP, short portNum)
{
	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error");
	}

	_socket = socket(PF_INET, SOCK_STREAM, 0);
	_recvTimeout = 1000;  // 1√ .
	setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&_recvTimeout, sizeof(_recvTimeout));


	if (_socket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error");
	}
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = inet_addr(IP.c_str());
	_servAdr.sin_port = htons(portNum);
}

int ClientLogic::Connect()
{
	if (connect(_socket, (SOCKADDR*)&_servAdr, sizeof(_servAdr)) == SOCKET_ERROR)
	{
		printf("WSAGetLastError() : %d\n", WSAGetLastError());
		ErrorHandling("connect() error");
	}
	else
	{
		puts("connected..");
		return 0;
	}
}

void ClientLogic::ExitClient()
{
	closesocket(_socket);
	WSACleanup();
}



void ClientLogic::SetName(std::string name)
{
	_name = name;
}
std::string ClientLogic::GetName()
{
	return _name;
}

void ErrorHandling(const char* msg)
{
	fputs(msg, stderr);
	fputs("\n", stderr);
	exit(1);
}

int ClientLogic::SendPacket(PacketIndex type, const char* buffer)
{
	if (buffer == NULL)
		return 0;

	switch (type)
	{
	case PacketIndex::ROOM_LIST:
	{
		PacketRoomList packetRoomList;
		send(_socket, (const char*)&packetRoomList, packetRoomList.headerSize, 0);
		return 0;
	}
	case PacketIndex::Login:
	{
		send(_socket, buffer, sizeof(PacketLogin), 0);
		break;
	}

	case PacketIndex::MAKE_ROOM:
	{
		send(_socket, buffer, sizeof(PacketMakeRoom), 0);
		RES_PacketMakeRoom resPacketMakeRoom;
		Sleep(100);
		int recvLen = recv(_socket, (char*)&resPacketMakeRoom, sizeof(RES_PacketMakeRoom), 0);
		while (recvLen < sizeof(RES_PacketMakeRoom))
		{
			recvLen += recv(_socket, (char*)&resPacketMakeRoom, 1, 0);
		}
		std::cout << resPacketMakeRoom.roomNum << " make " << std::endl;
		return resPacketMakeRoom.roomNum;
	}

	case PacketIndex::ENTER_ROOM:
	{
		send(_socket, buffer, sizeof(PacketEnterRoom), 0);
		return 0;
	}

	case PacketIndex::CLOSE_ROOM:
	{
		send(_socket, buffer, sizeof(PacketCloseRoom), 0);
		return 0;
	}
	case PacketIndex::SEND_MESSAGE:
	{
		send(_socket, buffer, sizeof(PacketSendMessage), 0);
		return 0;
	}

	case PacketIndex::MAKE_CLIENT_ID_INFO:
	{
		send(_socket, buffer, sizeof(PacketClientIdInfo), 0);
		return 0;
	}
	default:
		break;
	}
}

char* ClientLogic::RecvPacket(int packetSize)
{
	std::fill_n(buf, 1024, 0);
	int recvLen = recv(_socket, (char*)buf, packetSize, 0);
	while (recvLen < packetSize)
	{
		recvLen += recv(_socket, (char*)buf[recvLen], 1, 0);
	}
	return buf;
}
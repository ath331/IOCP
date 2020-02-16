#include <stdio.h>
#include <string>
#include "packet.h"
#include "ClientLogic.h"

#pragma warning(disable:4996)

void ErrorHandling(const char* msg);

void ClientLogic::Init(std::string IP, short portNum)
{
	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error");
	}

	_socket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
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

void ClientLogic::SendPacket(PacketIndex type, const char buffer[])
{
	switch (type)
	{
	case PacketIndex::Login:
	{
		PacketLogin packetLogin;
		memcpy(&packetLogin,buffer,sizeof(PacketLogin));
		send(_socket, (const char*)&packetLogin, packetLogin.header.headerSize, 0);
	}
	break;

	case PacketIndex::MAKE_ROOM:
	{
		PacketMakeRoom packetMakeRoom;
		memcpy(&packetMakeRoom, buffer, sizeof(PacketMakeRoom));
		send(_socket, (const char*)&packetMakeRoom, packetMakeRoom.header.headerSize, 0);
		//TODO : 만든 방의정보(roomNum 정도?)를 서버로부터받기까지 기다리고 ChatRoomDialog띄우기
	}
	break;

	default:
		break;
	}
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
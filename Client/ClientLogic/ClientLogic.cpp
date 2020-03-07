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
	_recvTimeout = 1000;  // 1초.
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
		send(_socket, (const char*)&packetRoomList, packetRoomList.header.headerSize, 0);
		return 0;
	}
	case PacketIndex::Login:
	{
		//TODO : case별로 memcpy지우기

		//PacketLogin packetLogin;
		//memcpy(&packetLogin, buffer, sizeof(PacketLogin));
		//send(_socket, (const char*)&packetLogin, packetLogin.header.headerSize, 0);
		send(_socket, buffer, sizeof(PacketLogin), 0);
		break;
	}

	case PacketIndex::MAKE_ROOM:
	{
		PacketMakeRoom packetMakeRoom;
		memcpy(&packetMakeRoom, buffer, sizeof(PacketMakeRoom));
		send(_socket, (const char*)&packetMakeRoom, packetMakeRoom.header.headerSize, 0);
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
		PacketEnterRoom packetEnterRoom;
		memcpy(&packetEnterRoom, buffer, sizeof(PacketEnterRoom));
		send(_socket, (const char*)&packetEnterRoom, packetEnterRoom.header.headerSize, 0);
		return 0;
	}

	case PacketIndex::CLOSE_ROOM:
	{
		PacketCloseRoom packetCloseRoom;
		memcpy(&packetCloseRoom, buffer, sizeof(PacketCloseRoom));
		send(_socket, (const char*)&packetCloseRoom, packetCloseRoom.header.headerSize, 0);
		return 0;
	}
	case PacketIndex::SEND_MESSAGE:
	{
		PacketSendMessage packetSendMessage;
		memcpy(&packetSendMessage, buffer, sizeof(PacketSendMessage));
		send(_socket, (const char*)&packetSendMessage, packetSendMessage.header.headerSize, 0);
		return 0;
	}

	case PacketIndex::MAKE_CLIENT_ID_INFO:
	{
		PacketClientIdInfo packetClientIdInfo;
		memcpy(&packetClientIdInfo, buffer, sizeof(PacketClientIdInfo));
		send(_socket, (const char*)&packetClientIdInfo, packetClientIdInfo.header.headerSize, 0);
		return 0;
	}
	default:
		break;
	}
}

//TODO : templete화 시키기
char* ClientLogic::RecvPacket(PacketIndex type)
{
	std::fill_n(buf, 1024, 0);
	switch (type)
	{
	case PacketIndex::DB_INSERT_DATA:
	{
		int recvLen = recv(_socket, (char*)buf, sizeof(PacketDBInsertData), 0);
		while (recvLen < sizeof(PacketDBInsertData))
		{
			recvLen += recv(_socket, (char*)buf[recvLen], 1, 0);
		}
		return buf;
	}

	case PacketIndex::Login:
	{
		int recvLen = recv(_socket, (char*)buf, sizeof(PacketLogin), 0);
		while (recvLen < sizeof(PacketLogin))
		{
			recvLen += recv(_socket, (char*)buf[recvLen], 1, 0);
		}
		return buf;
	}

	default:
		break;
	}
}


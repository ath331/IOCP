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

	//while (1)
	//{
	//	fputs("Input message(Q is quit) : ", stdout);
	//	fgets(_message, BUF_SIZE, stdin);
	//	if (!strcmp(_message, "q\n") || !strcmp(_message, "Q\n"))
	//		break;

	//	_strLen = strlen(_message);
	//	send(_hSocket, _message, _strLen, 0);
	//	_readLen = 0;

	//	while (1)
	//	{
	//		_readLen += recv(_hSocket, &_message[_readLen], BUF_SIZE - 1, 0);
	//		if (_readLen >= _strLen)
	//		{
	//			break;
	//		}
	//	}
	//	_message[_strLen] = 0; //�������� null���̱�
	//	printf("Message from Server : %s", _message);
	//}
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

void ClientLogic::SendPacket(SendType type)
{
	switch (type)
	{
	case SendType::Login:
	{
		PacketLogin packetLogin;
		strcpy((char*)packetLogin.name, _name.c_str());
		send(_socket, (const char*)&packetLogin, packetLogin.header.headerSize, 0);
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
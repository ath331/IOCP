#include <stdio.h>
#include <string>
#include <iostream>
#include "ClientLogic.h"

#pragma warning(disable:4996)

void ErrorHandling(const char* msg);

void ClientLogic::Init(std::string IP, short portNum)
{
	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		ErrorHandling("WSAStartup() error");
	}

	_socket = socket(PF_INET, SOCK_STREAM, 0);

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
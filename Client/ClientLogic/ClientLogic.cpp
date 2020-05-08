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
	//_recvTimeout = 1000;  // 1√ .
	//setsockopt(_socket, SOL_SOCKET, SO_RCVTIMEO, (char*)&_recvTimeout, sizeof(_recvTimeout));


	if (_socket == INVALID_SOCKET)
	{
		ErrorHandling("socket() error");
	}
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = inet_addr(IP.c_str());
	_servAdr.sin_port = htons(portNum);

	std::cout << "Server IP : " << IP << std::endl;
	std::cout << "Server PortNum : " << portNum << std::endl;
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

void ClientLogic::SendPacket(int bufferSize, const char* buffer)
{
	if (buffer == NULL)
		return;

	send(_socket, buffer, bufferSize, 0);
}

void ClientLogic::RecvPacket(int packetSize)
{
	Sleep(100);
	std::fill_n(buf, 4096, 0);
	int recvLen = recv(_socket, (char*)buf, packetSize, 0);
	while (recvLen < packetSize && 0 < recvLen)
	{
		recvLen += recv(_socket, (char*)buf[recvLen], 1, 0);
	}
}
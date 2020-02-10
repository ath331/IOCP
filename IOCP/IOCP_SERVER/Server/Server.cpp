#include "Server.h"
#include "../Basic/OVERLAPPED.h"
#include "../Basic/ClientInfo.h"

#include <iostream>

void Server::InputPortNum()
{
	std::cout << "Input PortNum : ";
	std::cin >> _portNum;
}

void Server::InitServer()
{
	GetSystemInfo(&_sysInfo);

	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		std::cout << "WSAStartup() error";
		exit(1);
	}

	_comPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_threadManager.InitThreadManager(_sysInfo.dwNumberOfProcessors, _comPort);
	_threadManager.MakeThread();

	_servSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAdr.sin_port = _portNum;

	if (bind(_servSock, (SOCKADDR*)&_servAdr, sizeof(_servAdr)) == SOCKET_ERROR)
	{
		std::cout << "bind() error";
		exit(1);
	}
	if (listen(_servSock, 5) == SOCKET_ERROR)
	{
		std::cout << "listen() error";
		exit(1);
	}
}


void Server::RunServer()
{
	while (1)
	{
		SOCKET clntSock;
		SOCKADDR_IN clientAdr;
		ClientInfo* clientInfo;
		Overlapped* ioInfo;

		int addrLen = sizeof(clientAdr);
		clntSock = accept(_servSock, (SOCKADDR*)&clientAdr, &addrLen);
		clientInfo = new ClientInfo();
		clientInfo->clientSock = clntSock;
		memcpy(&(clientInfo->clientAdr), &clientAdr, addrLen);

		CreateIoCompletionPort((HANDLE)clntSock, _comPort, (ULONG_PTR)clientInfo, 0);

		ioInfo = new Overlapped();
		ioInfo->Init();
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = Overlapped::IO_TYPE::READ;
		WSARecv(clientInfo->clientSock, &(ioInfo->wsaBuf), 1, (LPDWORD)&_recvBytes, (LPDWORD)&_flags, &(ioInfo->overlapped), NULL);
	}
}

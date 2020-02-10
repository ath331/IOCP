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
		//ErrorHandling("WSAStartup() error");
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
		SOCKADDR_IN clntAdr;
		ClientInfo* handleInfo;
		Overlapped* ioInfo;

		int addrLen = sizeof(clntAdr);
		clntSock = accept(_servSock, (SOCKADDR*)&clntAdr, &addrLen);
		handleInfo = new ClientInfo();
		handleInfo->hClntSock = clntSock;
		memcpy(&(handleInfo->clntAdr), &clntAdr, addrLen);

		CreateIoCompletionPort((HANDLE)clntSock, _comPort, (ULONG_PTR)handleInfo, 0);

		ioInfo = new Overlapped();
		ioInfo->Init();
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		ioInfo->rwMode = Overlapped::IO_TYPE::READ;
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, (LPDWORD)&_recvBytes, (LPDWORD)&_flags, &(ioInfo->overlapped), NULL);
	}
}

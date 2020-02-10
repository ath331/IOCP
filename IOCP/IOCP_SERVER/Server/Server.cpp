#include "Server.h"
#include "../ThreadManager/ThreadManager.h"
#include "../Basic/OVERLAPPED.h"
#include "../Basic/ClientInfo.h"

#include <iostream>

const static int BUF_SIZE = 100;

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
	_threadManager->InitThreadManager(_sysInfo.dwNumberOfProcessors, _comPort);
	_threadManager->MakeThread();

	_servSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAdr.sin_port = _portNum;

	bind(_servSock, (SOCKADDR*)&_servAdr, sizeof(_servAdr));
	listen(_servSock, 5);
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
		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->rwMode = Overlapped::IO_TYPE::READ;
		WSARecv(handleInfo->hClntSock, &(ioInfo->wsaBuf), 1, (LPDWORD)&_recvBytes, (LPDWORD)&_flags, &(ioInfo->overlapped), NULL);
	}
}

#include "Server.h"
#include "OverlappedCustom.h"
#include "ClientInfo.h"

#include "DB.h"
#include "ClientManager/ClientManager.h"
#include "../ThreadManager/ThreadManager.h"
#include "Acceptor/Acceptor.h"

#include <iostream>

const static int MAX_CLIENT_ACCEPT_NUM = 10;

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

	_threadManager = new ThreadManager;
	_clientManager = new ClientManager;
	_db = new DB;

	_comPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	_threadManager->InitThreadManager(_sysInfo.dwNumberOfProcessors * 2, _comPort, _clientManager, _db);

	_servSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAdr.sin_port = htons(_portNum);

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

	_acceptor = new Acceptor(_servSock);
}


void Server::RunServer()
{
	_threadManager->MakeThread();
	/*while (1)
	{
		ClientSocketInfo* clientSocketInfo;
		Overlapped* ioInfo;

		clientSocketInfo = new ClientSocketInfo();
		int addrLen = sizeof(clientSocketInfo->clientAdr);
		clientSocketInfo->clientSock = accept(_servSock, (SOCKADDR*)&(clientSocketInfo->clientAdr), &addrLen);

		CreateIoCompletionPort((HANDLE)clientSocketInfo->clientSock, _comPort, (ULONG_PTR)clientSocketInfo, 0);

		ioInfo = new Overlapped(Overlapped::IO_TYPE::RECV);
		ioInfo->wsaBuf.buf = ioInfo->buffer;
		WSARecv(clientSocketInfo->clientSock, &(ioInfo->wsaBuf), 1, (LPDWORD)&_recvBytes, (LPDWORD)&_flags, &(ioInfo->overlapped), NULL);
	}*/
	for (int i = 0; i < MAX_CLIENT_ACCEPT_NUM; i++)
		_acceptor->AcceptClient();

	_threadManager->WaitThread();
}


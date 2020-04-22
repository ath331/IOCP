#include "Server.h"
#include "OverlappedCustom.h"

#include "DB.h"
#include "Log/Log.h"
#include "ThreadManager.h"
#include "../BaseHeader.h"

#pragma warning(disable:4996)

const static int MAX_CLIENT_ACCEPT_NUM = 10;

void Server::InputPortNum()
{
	std::cout << "Input PortNum : ";
	std::cin >> _portNum;
}

Server::Server()
{
	GetSystemInfo(&_sysInfo);

	if (WSAStartup(MAKEWORD(2, 2), &_wsaData) != 0)
	{
		Log log(LogIndex::Error, "WSAStartup() error");
	}

	_InitManagers();

	_comPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	_servSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(&_servAdr, 0, sizeof(_servAdr));
	_servAdr.sin_family = AF_INET;
	_servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
	_servAdr.sin_port = htons(_portNum);

	_PrintServerInfo();

	if (bind(_servSock, (SOCKADDR*)&_servAdr, sizeof(_servAdr)) == SOCKET_ERROR)
	{
		Log log(LogIndex::Error, "bind() error");
	}
	if (listen(_servSock, 5) == SOCKET_ERROR)
	{
		Log log(LogIndex::Error, "listen() error");
	}

	CreateIoCompletionPort((HANDLE)_servSock, _comPort, NULL, 0);
	_acceptor = new Acceptor(_servSock);
	_threadManager->InitThreadManager(_sysInfo.dwNumberOfProcessors * 2, _comPort, _clientManager, _db, _acceptor, _roomManager);
}

void Server::_InitManagers()
{
	_db = new DB;
	_threadManager = new ThreadManager;
	_clientManager = new ClientManager;
	_roomManager = new RoomManager;
}


void Server::RunServer()
{
	_threadManager->MakeThread();

	for (int i = 0; i < MAX_CLIENT_ACCEPT_NUM; i++)
		_acceptor->AcceptClient();

	_threadManager->WaitThread();
}

void Server::_PrintServerInfo()
{
	_PrintInternalIP();
	_PrintExternalIP();
	cout << "Server PortNum : " << _portNum << endl;
}


void Server::_PrintInternalIP()
{
	PHOSTENT hostinfo;
	char hostname[50] = { 0, };
	char ipAddr[50] = { 0, };
	if (gethostname(hostname, sizeof(hostname)) == 0)
	{
		hostinfo = gethostbyname(hostname);
		strcpy(ipAddr, inet_ntoa(*(in_addr*)(hostinfo->h_addr_list[0])));
	}

	std::cout << "Server Internal IP : " << ipAddr << std::endl;
}

void Server::_PrintExternalIP()
{

}

Server::~Server()
{
	delete _db;

	delete _clientManager;
	delete _threadManager;
	delete _roomManager;

	delete _acceptor;
}
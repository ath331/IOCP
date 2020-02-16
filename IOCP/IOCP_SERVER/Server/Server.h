#pragma once

#include <WinSock2.h>
#include <windows.h>

//#include "ClientManager/ClientManager.h"
#include "../ThreadManager/ThreadManager.h"

class Server
{
public:
	void InputPortNum();
	void InitServer();
	void RunServer();

private:
	SYSTEM_INFO _sysInfo;
	WSADATA _wsaData;

	HANDLE _comPort;
	SOCKET _servSock;
	SOCKADDR_IN _servAdr;

	//ClientManager _clientManager;
	ThreadManager _threadManager;

	int _portNum = 9999;
	int _recvBytes = 0, _flags = 0;

};


#pragma once

#include <WinSock2.h>
#include <windows.h>

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

	ThreadManager _threadManager;

	int _portNum = 0;
	int _recvBytes = 0, _flags = 0;

};


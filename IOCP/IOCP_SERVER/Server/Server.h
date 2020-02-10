#pragma once

#include <WinSock2.h>
#include <windows.h>

class Server
{
public:
	void InputPortNum();
	void InitServer();
	void RunServer();
private:
	WSADATA _wsaData;
	HANDLE _comPort;
	SOCKET _servSock;
	SOCKADDR_IN _servAdr;

	int _portNum;
};


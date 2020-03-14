#pragma once
//TODO : IOCP send ´Ùµë±â
#include <WinSock2.h>
#include <windows.h>

class DB;
class ClientManager;
class ThreadManager;
class Acceptor;
class Server
{
public:
	void InputPortNum();
	void InitServer();
	void RunServer();
	void CloseServer();
private:
	SYSTEM_INFO _sysInfo;
	WSADATA _wsaData;

	HANDLE _comPort;
	SOCKET _servSock;
	SOCKADDR_IN _servAdr;

	DB* _db;
	ClientManager* _clientManager;
	ThreadManager* _threadManager;

	Acceptor* _acceptor;

	int _portNum = 9999;
	int _recvBytes = 0, _flags = 0;

};


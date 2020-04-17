#pragma once
#include <WinSock2.h>
#include <windows.h>

class DB;
class ClientManager;
class ThreadManager;
class Acceptor;
class RoomManager;
class Server
{
public:
	Server();
	~Server();

	void InputPortNum();
	void RunServer();
private:
	SYSTEM_INFO _sysInfo;
	WSADATA _wsaData;

	HANDLE _comPort;
	SOCKET _servSock;
	SOCKADDR_IN _servAdr;

	DB* _db;
	ClientManager* _clientManager;
	ThreadManager* _threadManager;
	RoomManager* _roomManager;

	Acceptor* _acceptor;

	int _portNum = 9999;
	int _recvBytes = 0, _flags = 0;

	void _InitManagers();
};
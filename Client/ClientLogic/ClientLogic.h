#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>

const static int BUF_SIZE = 1024;

class ClientLogic
{
public:
	void Init(std::string IP,short portNum);
	void Connect();
	void ExitClient();
private:
	WSADATA _wsaData;
	SOCKET _socket;
	SOCKADDR_IN _servAdr;

	char _message[BUF_SIZE];
	int _strLen = 0, _readLen = 0;
};
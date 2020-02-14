#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>

const static int BUF_SIZE = 1024;

enum class SendType : int
{
	Login,
};

class ClientLogic
{
public:
	void Init(std::string IP,short portNum);
	int Connect();
	void ExitClient();

	void SetName(std::string name);
	std::string GetName();
	void SendPacket(SendType type);
private:
	WSADATA _wsaData;
	SOCKET _socket;
	SOCKADDR_IN _servAdr;

	std::string _name;
	char _message[BUF_SIZE];
	int _strLen = 0, _readLen = 0;
};
#pragma once
#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include <string>

const static int BUF_SIZE = 1024;

enum class PacketIndex;
class ClientLogic
{
public:
	void Init(std::string IP,short portNum);
	int Connect();
	void ExitClient();

	void SetName(std::string name);
	std::string GetName();
	void SendPacket(PacketIndex type, const char buffer[]);
private:
	WSADATA _wsaData;
	SOCKET _socket;
	SOCKADDR_IN _servAdr;

	std::string _name;
	int _strLen = 0, _readLen = 0;
};
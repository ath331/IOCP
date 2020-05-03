#pragma once
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <string>
#define MWM_SOCKET (WM_USER+1)

const static int BUF_SIZE = 1024;

enum class PacketIndex;
class ClientLogic
{
public:
	void Init(std::string IP, short portNum);
	int Connect();
	void ExitClient();

	void SetName(std::string name);
	std::string GetName();

	char buf[4096] = { 0, };
	char* RecvPacket(int packetSize);
	void SendPacket(int bufferSize, const char* buffer);

	SOCKET _socket;

	bool GetIsEnteredRoom()
	{
		return _isEnteredRoom;
	}
	void SetIsEnteredRoom(bool type)
	{
		_isEnteredRoom = type;
	}
private:
	WSADATA _wsaData;
	SOCKADDR_IN _servAdr;

	DWORD _recvTimeout;
	std::string _name;
	int _strLen = 0, _readLen = 0;

	bool _isEnteredRoom = FALSE;
};

//#include "ClientLogic.hpp"
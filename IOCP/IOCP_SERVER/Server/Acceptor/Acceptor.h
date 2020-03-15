#pragma once
#include "OverlappedCustom.h"

class Acceptor
{
public:
	Acceptor(SOCKET servSock) : _servSock(servSock), _overlapped(Overlapped::IO_TYPE::ACCEPT) {}
	~Acceptor() {}

	void AcceptClient();
	void GetClientSockAddr(SOCKADDR_IN* local);
	SOCKET GetClientSock() { return _clientSock; };
private:
	SOCKET _servSock;
	SOCKET _clientSock;

	Overlapped _overlapped;
	char _buff[1024];
	DWORD _recvLen;
};


#pragma once
#include "OverlappedCustom.h"

class Acceptor
{
public:
	Acceptor() {};
	Acceptor(SOCKET servSock) :_servSock(servSock) 
	{
		_overlapped.ioType = Overlapped::IO_TYPE::ACCEPT;
	};

	void AcceptClient();
	void GetClientSockAddr(SOCKADDR_IN* local);
private:
	SOCKET _servSock;
	SOCKET _clientSock;

	Overlapped _overlapped;
	char _buff[1024];
	DWORD _recvLen;
};


#include "Acceptor.h"
#include <MSWSock.h>
#include <iostream>

#pragma comment(lib,"mswsock.lib")

void Acceptor::AcceptClient()
{
	_clientSock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	_overlapped.Init();
	memset(_buff, 0, sizeof(_buff));
	_recvLen = 0;

	if (AcceptEx(_servSock, _clientSock, _buff, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &_recvLen,
		(LPOVERLAPPED)&_overlapped) == false)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			std::cout << "AcceptEx() error" << std::endl;
			std::cout << errorCode << std::endl;
		}
	}
}

void Acceptor::_GetClientSockAddr()
{
	SOCKADDR_IN* localAddr = nullptr;
	int localSockLen = 0;
	SOCKADDR_IN* remoteAddr = nullptr;
	int remoteSockLen = 0;

	GetAcceptExSockaddrs(_buff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		(SOCKADDR**)&localAddr, &localSockLen,
		(SOCKADDR**)&remoteAddr, &remoteSockLen
	);
}
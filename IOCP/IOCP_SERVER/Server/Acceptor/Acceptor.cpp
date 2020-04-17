#include "Acceptor.h"
#include "Log/Log.h"

#include <MSWSock.h>
#include <iostream>

#pragma comment(lib,"mswsock.lib")

void Acceptor::AcceptClient()
{
	_overlapped.Init();
	_overlapped.sock = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	memset(_buff, 0, sizeof(_buff));
	_recvLen = 0;

	if (AcceptEx(_servSock, _overlapped.sock, _buff, 0,
		sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &_recvLen,
		(LPOVERLAPPED)&_overlapped) == false)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			Log log(LogIndex::WARNING, "AcceptEx() error", errorCode);
		}
	}
}


void Acceptor::GetClientSockAddr(SOCKADDR_IN* local)
{
	SOCKADDR_IN* localAddr = nullptr;
	int localSockLen = 0;
	SOCKADDR_IN* remoteAddr = nullptr;
	int remoteSockLen = 0;

	GetAcceptExSockaddrs(_buff, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16,
		(SOCKADDR**)&localAddr, &localSockLen,
		(SOCKADDR**)&remoteAddr, &remoteSockLen
	);

	local->sin_addr = localAddr->sin_addr;
	local->sin_port = localAddr->sin_port;
}
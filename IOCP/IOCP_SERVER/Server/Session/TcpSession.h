#pragma once
#include "Recver/Recver.h"

class TcpSession
{
public:
	TcpSession(HANDLE cpHandle, SOCKET sock) : _cpHandle(cpHandle),_sock(sock)
	, _recvOverlapped(Overlapped::IO_TYPE::RECV) {};
	void PostRecv();
private:
	HANDLE _cpHandle;
	SOCKET _sock;
	SOCKADDR_IN _local;
	SOCKADDR_IN _remote;

	Overlapped _recvOverlapped;
	WSABUF _recvBuff;
	unsigned long _recvFlag;
	unsigned int _recvBuffSize;
	unsigned int _recvBuffOffSet;
	unsigned int _recvLen;
};
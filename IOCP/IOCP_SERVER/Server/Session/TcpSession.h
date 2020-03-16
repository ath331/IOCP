#pragma once
#include "OverlappedCustom.h"

class TcpSession
{
public:
	TcpSession(HANDLE cpHandle, SOCKET sock) : _cpHandle(cpHandle), _sock(sock)
		, _recvOverlapped(Overlapped::IO_TYPE::RECV) 
	{
		memset(&_recvBuff, 0, sizeof(WSABUF));
		CreateIoCompletionPort((HANDLE)sock, _cpHandle, sock, 0);
	};
	void PostRecv();
	void CheckPcketSize();
private:
	SOCKET _sock;
	HANDLE _cpHandle;

	Overlapped _recvOverlapped;
	WSABUF _recvBuff;
	unsigned long _recvFlag = 0;
	unsigned int _recvBuffSize = 0;
	unsigned int _recvBuffOffSet = 0;
	unsigned int _recvLen = 0;
};
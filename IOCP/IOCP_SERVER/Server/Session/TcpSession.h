#pragma once
#include "OverlappedCustom.h"

const static int MAX_BUF_SIZE = 1024;

class TcpSession
{
public:
	TcpSession(HANDLE cpHandle, SOCKET sock) : _cpHandle(cpHandle), _sock(sock)
		, _recvOverlapped(Overlapped::IO_TYPE::RECV)
	{
		_recvBuff.len = 1024;
		_recvBuff.buf = new char[MAX_BUF_SIZE];
		CreateIoCompletionPort((HANDLE)sock, _cpHandle, sock, 0);
	};
	~TcpSession();
	void PostRecv();
	void CheckPcketSize(int recvTransLen);
private:
	SOCKET _sock;
	HANDLE _cpHandle;

	Overlapped _recvOverlapped;
	WSABUF _recvBuff;
	unsigned long _recvFlag = 0;
	unsigned int _recvLen = 0;
	unsigned int _recvTotalLen = 0;
	unsigned int _recvLenOffSet = 0;
};
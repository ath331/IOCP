#pragma once
#include "packet.h"
#include "OverlappedCustom.h"
#include <concurrent_queue.h>

const static int MAX_BUF_SIZE = 1024;

using namespace concurrency;

class TcpSession
{
public:
	TcpSession(HANDLE cpHandle, SOCKET sock, concurrent_queue<PacketInfo>* packetQueue, concurrent_queue<PacketInfo>* packetDBQueue) :
		_cpHandle(cpHandle), _sock(sock), _packetQueue(packetQueue), _packetDBQueue(packetDBQueue)
		, _recvOverlapped(Overlapped::IO_TYPE::RECV)
		, _sendOverlapped(Overlapped::IO_TYPE::SEND)
	{
		_recvBuff.len = 1024;
		_recvBuff.buf = new char[MAX_BUF_SIZE];

		CreateIoCompletionPort((HANDLE)sock, _cpHandle, sock, 0);
	};
	~TcpSession();

	void PostRecv();
	void CheckPcketSize(int recvTransLen);

	void PostSend();
private:
	SOCKET _sock;
	HANDLE _cpHandle;
	concurrent_queue<PacketInfo>* _packetDBQueue;
	concurrent_queue<PacketInfo>* _packetQueue;

	Overlapped _recvOverlapped;
	WSABUF _recvBuff;
	unsigned long _recvFlag		= 0;
	unsigned int _recvLen		= 0;
	unsigned int _recvTotalLen	= 0;
	unsigned int _recvLenOffSet = 0;

	Overlapped _sendOverlapped;
	WSABUF _sendBuff;
};
#pragma once
#include "packet.h"
#include "Lock.h"
#include "OverlappedCustom.h"
#include <concurrent_queue.h>
#include <vector>

const static int MAX_BUF_SIZE = 1024;

using namespace concurrency;
using namespace std;

class TcpSession
{
public:
	TcpSession(HANDLE cpHandle, SOCKET sock, concurrent_queue<PacketInfo>* packetQueue, concurrent_queue<PacketInfo>* packetDBQueue) :
		_cpHandle(cpHandle), _sock(sock), _packetQueue(packetQueue), _packetDBQueue(packetDBQueue)
		, _recvOverlapped(Overlapped::IO_TYPE::RECV)
		, _sendOverlapped(Overlapped::IO_TYPE::SEND)
	{
		_recvBuf.len = 0;
		_recvBuf.buf = new char[MAX_BUF_SIZE];

		CreateIoCompletionPort((HANDLE)sock, _cpHandle, sock, 0);
	};
	~TcpSession();

	void PostRecv();
	void OnRecvForIocp(int recvTransLen); //For IOCP RECV 

	void PushSendVec(PacketInfo pck, ULONG pckSize);
	void OnSendForIocp();
private:
	SOCKET _sock;
	HANDLE _cpHandle;
	concurrent_queue<PacketInfo>* _packetDBQueue;
	concurrent_queue<PacketInfo>* _packetQueue;

	Overlapped _recvOverlapped;
	WSABUF _recvBuf;
	unsigned long _recvFlag		= 0;
	unsigned int _recvLen		= 0;
	unsigned int _recvLenOffSet = 0;

	Overlapped _sendOverlapped;
	unsigned int _sendLen		= 0;
	vector<Pck> _packetSendVec;
	vector<Pck> _packetTempSendVec;
	bool _isSending				= false;
	void _PostSend();
	Lock _sendLock;
};
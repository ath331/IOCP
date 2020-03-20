#include "TcpSession.h"
#include <iostream>

using namespace std;
TcpSession::~TcpSession()
{
	delete _recvBuff.buf;
}

void TcpSession::PostRecv()
{
	if (SOCKET_ERROR == WSARecv(_sock, &_recvBuff, 1, (DWORD*)&_recvLen, &_recvFlag, (LPOVERLAPPED)&_recvOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			cout << error << " TcpSession PostRecv() WSARecv() error" << endl;
		}
	}
}

void TcpSession::CheckPcketSize(int recvTransLen)
{
	//TODO : 데이터길이가 패킷의 길이보다 많이오면?
	_recvLenOffSet += recvTransLen;
	if (sizeof(_recvBuff.buf) < sizeof(PacketHeader))
	{
		PostRecv();
		return;
	}

	PacketHeader* packetHeader = reinterpret_cast<PacketHeader*>(_recvBuff.buf);
	_recvTotalLen = packetHeader->headerSize;
	if (_recvLenOffSet < _recvTotalLen)
	{
		PostRecv();
		return;
	}

	PacketInfo tempPacketInfo = { _sock, packetHeader->index, (const char*)_recvBuff.buf };
	if (packetHeader->index < PacketIndex::DB_INDEX)
	{
		_packetQueue->push(tempPacketInfo);
	}
	else if (packetHeader->index > PacketIndex::DB_INDEX)
	{
		_packetDBQueue->push(tempPacketInfo);
	}

	_recvTotalLen -= packetHeader->headerSize;
	PostRecv();
}

void TcpSession::PostSend()
{
	if (SOCKET_ERROR == WSASend(_sock, &_sendBuff, 1, (DWORD*)sizeof(&_sendBuff.buf), 0, (LPOVERLAPPED)&_sendOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			cout << error << " TcpSession PostSend() WSASend() error" << endl;
		}
	}
}

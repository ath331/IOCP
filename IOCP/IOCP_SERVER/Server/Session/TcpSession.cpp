#include "TcpSession.h"
#include "Log/Log.h"
#include <iostream>

using namespace std;
TcpSession::~TcpSession()
{
	delete _recvBuf.buf;
}

void TcpSession::PostRecv()
{
	_recvBuf.len = MAX_BUF_SIZE - _recvLenOffSet;

	if (SOCKET_ERROR == WSARecv(_sock, &_recvBuf, 1, (DWORD*)&_recvLen, &_recvFlag, (LPOVERLAPPED)&_recvOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			Log log(LogIndex::WARNING, "TcpSession PostRecv() WSARecv() error", error);
		}
	}
}

void TcpSession::OnRecvForIocp(int recvTransLen)
{
	_recvLenOffSet += recvTransLen;
	while (TRUE)
	{
		if (_recvLenOffSet < sizeof(PacketHeader))
		{
			PostRecv();
			return;
		}

		PacketHeader* packetHeader = reinterpret_cast<PacketHeader*>(_recvBuf.buf);
		unsigned int _recvTotalLen = packetHeader->headerSize;
		if (_recvBuf.len < _recvTotalLen)
		{
			char* tempBuf = new char[_recvTotalLen];
			memcpy(&tempBuf, _recvBuf.buf, _recvLenOffSet);
			delete[] _recvBuf.buf;
			_recvBuf.buf = tempBuf;
			_recvBuf.len = _recvTotalLen;

			Log log(LogIndex::WARNING, "Change Recv Buf Size : ", _recvLen);

			PostRecv();
			return;
		}

		if (_recvLenOffSet < _recvTotalLen)
		{
			PostRecv();
			return;
		}

		/*_recvBuf.buf에 packetHeader->headerSize 이상의 데이터가 있음.
		사용할 패킷의 크기만큼만 복사하고 초과의 데이터는 앞으로 땡긴다*/

		char* cpyRecvBuf = new char[_recvBuf.len];
		memcpy(cpyRecvBuf, _recvBuf.buf, _recvTotalLen);
		PacketInfo tempPacketInfo = { _sock, packetHeader->index, cpyRecvBuf };
		delete[] cpyRecvBuf;

		if (packetHeader->index < PacketIndex::DB_INDEX)
		{
			_packetQueue->push(tempPacketInfo);
		}
		else if (packetHeader->index > PacketIndex::DB_INDEX)
		{
			_packetDBQueue->push(tempPacketInfo);
		}

		_recvLenOffSet -= _recvTotalLen;
		memmove(_recvBuf.buf, _recvBuf.buf + _recvTotalLen, _recvLenOffSet);
	}
}

void TcpSession::PushSendVec(PacketInfo pck, ULONG pckSize)
{
	LockGuard sendLcokGuard(_sendLock);
	Pck packet = { pckSize,(CHAR*)pck.packetBuffer };
	_packetTempSendVec.push_back(packet);

	if (!_isSending)
		_PostSend();

	return;
}

void TcpSession::OnSendForIocp()
{
	LockGuard sendLcokGuard(_sendLock);
	_isSending = false;
	if (_packetTempSendVec.empty())
		return;
	_PostSend();
}


void TcpSession::_PostSend()
{
	LockGuard sendLcokGuard(_sendLock);
	_packetSendVec = _packetTempSendVec;
	_packetTempSendVec.clear();

	if (SOCKET_ERROR == WSASend(_sock, &_packetSendVec[0], static_cast<DWORD>(_packetSendVec.size()), (LPDWORD)&_sendLen, NULL, (LPOVERLAPPED)&_sendOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			Log log(LogIndex::WARNING, "TcpSession PostSend() WSASend() error", error);
		}
	}

	_isSending = true;
}

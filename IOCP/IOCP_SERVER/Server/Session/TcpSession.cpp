#include "TcpSession.h"
#include <iostream>

using namespace std;
TcpSession::~TcpSession()
{
	delete _recvBuf.buf;
}

void TcpSession::PostRecv()
{
	//TODO : buf조절하기
	_recvBuf.len = MAX_BUF_SIZE - _recvLenOffSet;

	if (SOCKET_ERROR == WSARecv(_sock, &_recvBuf, 1, (DWORD*)&_recvLen, &_recvFlag, (LPOVERLAPPED)&_recvOverlapped, NULL))
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
		if (MAX_BUF_SIZE < _recvTotalLen)
		{
			char* tempBuf = new char[_recvTotalLen];
			memcpy(&tempBuf, _recvBuf.buf, _recvLenOffSet);
			delete[] _recvBuf.buf;
			_recvBuf.buf = tempBuf;
			_recvBuf.len = _recvTotalLen;
			//TODO : cout으로 띄우지말고 error관련 class 만들기
			std::cout << "Change Recv Buf Size : " << _recvTotalLen << std::endl;
		}

		if (_recvLenOffSet < _recvTotalLen)
		{
			PostRecv();
			return;
		}

		/*_recvBuf.buf에 packetHeader->headerSize 이상의 데이터가 있음
		사용할 패킷의 크기만큼만 이동하고 초과의 데이터는 앞으로 땡긴다*/
		char moveRecvBuf[MAX_BUF_SIZE + 1];
		memmove(moveRecvBuf, _recvBuf.buf, _recvTotalLen);
		PacketInfo tempPacketInfo = { _sock, packetHeader->index, (const char*)moveRecvBuf };
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

void TcpSession::PushSendVec(PacketInfo pck,ULONG pckSize)
{
	Pck packet = { pckSize,(CHAR*)pck.packetBuffer };
	_packetSendQueue.push(packet);
	//TODO : queue를 사용하는 의미가 없다 (하나넣고 그냥 바로 사용해서?)
	_PostSend();
	return;
}

void TcpSession::_PostSend()
{
	if (_packetSendQueue.empty())
	{
		return;
	}

	Pck tempPacket;
	_packetSendQueue.try_pop(tempPacket);

	if (SOCKET_ERROR == WSASend(_sock, &tempPacket, 1, (LPDWORD)&_sendLen, NULL, (LPOVERLAPPED)&_sendOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			cout << error << " TcpSession PostSend() WSASend() error" << endl;
		}
	}

	isSending = true;
}

#include "TcpSession.h"
#include <iostream>

using namespace std;
TcpSession::~TcpSession()
{
	delete _recvBuf.buf;
}

void TcpSession::PostRecv()
{
	//TODO : buf�����ϱ�
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
			//TODO : cout���� ��������� error���� class �����
			std::cout << "Change Recv Buf Size : " << _recvTotalLen << std::endl;
		}

		if (_recvLenOffSet < _recvTotalLen)
		{
			PostRecv();
			return;
		}

		/*_recvBuf.buf�� packetHeader->headerSize �̻��� �����Ͱ� ����
		����� ��Ŷ�� ũ�⸸ŭ�� �̵��ϰ� �ʰ��� �����ʹ� ������ �����*/
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
	//TODO : queue�� ����ϴ� �ǹ̰� ���� (�ϳ��ְ� �׳� �ٷ� ����ؼ�?)
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

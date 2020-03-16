#include "TcpSession.h"
#include "packet.h"
#include <iostream>

using namespace std;

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

void TcpSession::CheckPcketSize()
{
	if (_recvBuff.len < sizeof(PacketHeader))
	{
		PostRecv();
		return;
	}

	PacketHeader packetHeader;
	memcpy(&packetHeader, &_recvBuff, sizeof(PacketHeader));
	switch (packetHeader.index)
	{
	case PacketIndex::Login:
	{
		cout << "Recv Login Packet" << endl;
	}
	break;

	default:
		break;
	}
	PostRecv();
}

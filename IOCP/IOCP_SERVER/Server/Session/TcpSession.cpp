#include "TcpSession.h"
#include <iostream>

void TcpSession::PostRecv()
{
	if (SOCKET_ERROR == WSARecv(_sock, &_recvBuff, 1, (DWORD*)&_recvLen, &_recvFlag, (LPOVERLAPPED)&_recvOverlapped, NULL))
	{
		int error = WSAGetLastError();
		if (error != WSA_IO_PENDING)
		{
			std::cout << "TcpSession PostRecv() WSARecv() error" << std::endl;
		}
	}
}

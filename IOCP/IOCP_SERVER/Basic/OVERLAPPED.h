#pragma once
#include <WinSock2.h>

const static int BUF_SIZE = 100;

struct Overlapped
{
	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];

	enum class IO_TYPE : int
	{
		READ,
		WRITE,
	};
	IO_TYPE rwMode;

	Overlapped()
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		wsaBuf.len = BUF_SIZE;
	}

};
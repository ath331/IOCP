#pragma once
#include <WinSock2.h>

const static int BUF_SIZE = 1024;

struct Overlapped
{
	enum class IO_TYPE : int
	{
		READ,
		WRITE,
	};

	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	IO_TYPE rwMode;

	Overlapped()
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
		wsaBuf.len = BUF_SIZE;
	}
};
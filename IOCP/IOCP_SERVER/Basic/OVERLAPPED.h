#pragma once
#include <WinSock2.h>

const static int BUF_SIZE = 100;

struct Overlapped : public WSAOVERLAPPED
{
	enum class IO_TYPE : int
	{
		READ,
		WRITE,
	};

	void Init()
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
	}

	OVERLAPPED overlapped;
	WSABUF wsaBuf;
	char buffer[BUF_SIZE];
	IO_TYPE rwMode;
};
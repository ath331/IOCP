#pragma once
#include <WinSock2.h>

const static int BUF_SIZE = 1024;

struct Overlapped
{
	enum class IO_TYPE : int
	{
		ACCEPT,
		RECV,
		SEND,
	};

	OVERLAPPED overlapped;
	IO_TYPE ioType;
	SOCKET sock;
	Overlapped() {}
	Overlapped(IO_TYPE _ioType) : ioType(_ioType)
	{
		Init();
	}

	void Init()
	{
		memset(&overlapped, 0, sizeof(OVERLAPPED));
	}
};
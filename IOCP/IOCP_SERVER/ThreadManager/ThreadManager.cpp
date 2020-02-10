#include "ThreadManager.h"

#include <process.h>
#include <iostream>

void ThreadManager::InitThreadManager(int maxThreadNum, HANDLE comPort)
{
	_maxThreadNum = maxThreadNum;
	_comPort = comPort;
}

void ThreadManager::MakeThread()
{
	_MakeIOThreads();
	_MakeLogicThread();
}


void ThreadManager::_MakeIOThreads()
{
	for (int i = 0; i < _maxThreadNum - 1; i++) //한개의 쓰레드는 SingleLogicThread로 사용
	{
		_beginthreadex(NULL, 0, _RunIOThreadMain, _comPort, 0, NULL);
	}
}

void ThreadManager::_MakeLogicThread()
{
	_beginthreadex(NULL, 0, _RunLogicThreadMain, NULL, 0, NULL);
}

unsigned int WINAPI ThreadManager::_RunIOThreadMain(HANDLE completionPort)
{
	HANDLE comPort = completionPort;
	SOCKET sock;
	DWORD bytesTrans;
	LPPER_HANDLE_DATA handleInfo;
	LPPER_IO_DATA ioInfo;
	DWORD flags = 0;

	while (1)
	{
		GetQueuedCompletionStatus(comPort, &bytesTrans, (PULONG_PTR)&handleInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = handleInfo->hClntSock;

		if (ioInfo->rwMode == IO_TYPE::READ)
		{
			std::cout << "message received!";
			if (bytesTrans == 0)
			{
				closesocket(sock);
				delete handleInfo;
				delete ioInfo;
				continue;
			}

			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = IO_TYPE::WRITE;
			WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);

			ioInfo = new PER_IO_DATA;
			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->rwMode = IO_TYPE::READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else
		{
			std::cout << "message sent!";
			delete ioInfo;
		}
	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunLogicThreadMain(HANDLE completionPortIO)
{

}

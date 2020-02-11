#include "ThreadManager.h"
#include "../Basic/ClientInfo.h"
#include "../Basic/OverlappedCustom.h"

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
	SOCKET sock;
	DWORD bytesTrans;
	ClientInfo* clientInfo;
	Overlapped* ioInfo;
	DWORD flags = 0;

	while (1)
	{
		GetQueuedCompletionStatus(completionPort, &bytesTrans, (PULONG_PTR)&clientInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = clientInfo->clientSock;

		if (ioInfo->rwMode == Overlapped::IO_TYPE::READ)
		{
			std::cout << "message received!\n";
			if (bytesTrans == 0)
			{
				closesocket(sock);
				continue;
			}

			memset(&(ioInfo->overlapped), 0, sizeof(OVERLAPPED));
			ioInfo->wsaBuf.len = bytesTrans;
			ioInfo->rwMode = Overlapped::IO_TYPE::WRITE;
			WSASend(sock, &(ioInfo->wsaBuf), 1, NULL, 0, &(ioInfo->overlapped), NULL);
		}
		else if (ioInfo->rwMode == Overlapped::IO_TYPE::WRITE)
		{
			std::cout << "message sent!\n";
			ioInfo->wsaBuf.buf = ioInfo->buffer;
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->rwMode = Overlapped::IO_TYPE::READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
		else
		{
			std::cout << "IO_TYPE is NULL\n";
			ioInfo->wsaBuf.len = BUF_SIZE;
			ioInfo->rwMode = Overlapped::IO_TYPE::READ;
			WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
		}
	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunLogicThreadMain(HANDLE completionPortIO)
{
	return 0;
}

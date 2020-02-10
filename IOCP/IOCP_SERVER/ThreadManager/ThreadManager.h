#pragma once
#include <WinSock2.h>
#include <windows.h>

class ThreadManager
{
public:
	void InitThreadManager(int maxThreadNum, HANDLE comPort);
	void MakeThread();
private:
	HANDLE _comPort;
	int _maxThreadNum=0;

	void _MakeIOThreads();
	void _MakeLogicThread();

	static unsigned int WINAPI _RunIOThreadMain(HANDLE completionPortIO);
	static unsigned int WINAPI _RunLogicThreadMain(HANDLE completionPortIO);
};


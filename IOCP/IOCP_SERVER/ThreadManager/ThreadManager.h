#pragma once
#include <WinSock2.h>
#include <windows.h>


const static int READ = 3;
const static int WRITE = 5;

class ThreadManager
{
public:
	void InitThreadManager(int maxThreadNum, HANDLE comPort);
	void MakeThread();
private:
	HANDLE _comPort;
	int _maxThreadNum;

	void _MakeIOThreads();
	void _MakeLogicThread();

	static unsigned int WINAPI _RunIOThreadMain(HANDLE completionPortIO);
	static unsigned int WINAPI _RunLogicThreadMain(HANDLE completionPortIO);
};


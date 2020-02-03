#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void* arg);

int main(int argc, char* argv[])
{
	HANDLE hThread;
	DWORD wr;
	unsigned threadID;
	int param = 5;

	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);
	if (hThread == 0)
	{
		std::cout << "_beginthreadex() error" << std::endl;
		return -1;
	}

	if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED)
	{
		std::cout << "WaitForSingleObject() error" << std::endl;
		return -1;
	}

	printf("wait result : %s\n", (wr == WAIT_OBJECT_0) ? "signaled" : "time-out");

	return 0;
}

unsigned WINAPI ThreadFunc(void* arg)
{
	int i = 0;
	int cnt = *((int*)arg);
	for (int i = 0; i < cnt; i++)
	{
		Sleep(1000);
		std::cout << "running Thread" << std::endl;
	}
	return 0;
}
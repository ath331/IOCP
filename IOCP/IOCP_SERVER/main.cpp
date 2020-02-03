#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>

#define THREAD_NUM 50
#define MAX_COUNT_NUM 500000
unsigned WINAPI ThreadInc(void* arg);
unsigned WINAPI ThreadDes(void* arg);
long long num = 0;

int main(int argc, char* argv[])
{
	HANDLE hThreads[THREAD_NUM];
	int i = 0;

	std::cout << "long long size : " << sizeof(long long) << std::endl;
	for (i = 0; i < THREAD_NUM; i++)
	{
		if (i % 2)
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadInc, NULL, 0, NULL);
		else
			hThreads[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadDes, NULL, 0, NULL);
	}

	WaitForMultipleObjects(THREAD_NUM, hThreads, TRUE, INFINITE);
	std::cout << "result : " << num << std::endl;
	return 0;
}

unsigned WINAPI ThreadInc(void* arg)
{
	int i = 0;
	for (i = 0; i < MAX_COUNT_NUM; i++)
	{
		num++;
	}
	return 0;
}

unsigned WINAPI ThreadDes(void* arg)
{
	int i = 0;
	for (i = 0; i < MAX_COUNT_NUM; i++)
	{
		num--;
	}
	return 0;
}

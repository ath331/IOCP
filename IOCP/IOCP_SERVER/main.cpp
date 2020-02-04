#include <iostream>
#include <stdio.h>
#include <Windows.h>
#include <process.h>

#define STR_LEN 100
unsigned WINAPI NumberOfA(void* arg);
unsigned WINAPI NumberOfOthers(void* arg);
static char str[STR_LEN];
static HANDLE hEvents;

int main(int argc, char* argv[])
{
	HANDLE hThread1, hThread2;
	hEvents = CreateEvent(NULL, TRUE, FALSE, NULL); //ManualMode
	hThread1 = (HANDLE)_beginthreadex(NULL, 0, NumberOfA, NULL, 0, NULL);
	hThread2 = (HANDLE)_beginthreadex(NULL, 0, NumberOfOthers, NULL, 0, NULL);

	std::cout << "Input String : ";
	fgets(str, STR_LEN, stdin);
	SetEvent(hEvents);// N.S -> S

	WaitForSingleObject(hThread1, INFINITE);
	WaitForSingleObject(hThread2, INFINITE);
	ResetEvent(hEvents);
	CloseHandle(hEvents);

	return 0;
}

unsigned WINAPI NumberOfA(void* arg)
{
	int cnt = 0;
	WaitForSingleObject(hEvents, INFINITE);
	for (int i = 0; i < str[i] != 0; i++)
	{
		if (str[i] == 'A')
			cnt++;
	}
	std::cout << "Num Of 'A' : " << cnt << std::endl;
	return 0;
}

unsigned WINAPI NumberOfOthers(void* arg)
{
	int cnt = 0;
	WaitForSingleObject(hEvents, INFINITE);
	for (int i = 0; i < str[i] != 0; i++)
	{
		if (str[i] != 'A')
			cnt++;
	}
	std::cout << "Num Of Others : " << cnt-1 << std::endl; //마지막 NULL값 까지 ++해서 -1을 하는듯
	return 0;
}

//스레드가 동시에 실행 되서 출력이 이쁘게 안나올수도있다.
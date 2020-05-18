#include "ThreadManager.h"
#include <iostream>

void ThreadManager::Init()
{
	_threadNum = -1;
	_SetThreadNum();
	for (int i = 0; i < _threadNum; i++)
	{
		TestClient* test = new TestClient;

		_threadVec.push_back(thread(&TestClient::RunTestClient, test));
	}
}

void ThreadManager::Start()
{
	for (int i = 0; i < _threadNum; i++)
	{
		_threadVec[i].join();
	}
}

void ThreadManager::_SetThreadNum()
{
	while (true)
	{
		cout << "Input : Run Thread Count = ";
		cin >> _threadNum;

		if (_threadNum > 0)
			break;
	}
}
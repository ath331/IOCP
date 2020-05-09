#pragma once
#include <vector>
#include <thread>
#include "../Client/TestClient.h"

using namespace std;

class ThreadManager
{
public:
	void Init();
	void Start();

private:
	void _SetThreadNum();

	unsigned int _threadNum;

	vector<thread> _threadVec;
};


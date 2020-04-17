#pragma once
#include <iostream>

using namespace std;

enum class LogIndex : int
{
	WARNING,
	Error,
	LOG,
};

class Log
{
public:
	Log(LogIndex index, const char* errStr, int errNo = 0)
	{
		switch (index)
		{
		case LogIndex::WARNING:
			cout << "Warning : " << errNo << " " << errStr << endl;
			break;

		case LogIndex::LOG:
			if (_isLogMessage)
				cout << "log : " << errStr << endl;
			break;

		case LogIndex::Error:
			cout << "Error : " << errNo << " " << errStr << endl;
			exit(1);
			break;

		default:
			break;
		}
	}

private:
	/*
	서버성능 테스트를 위해서 더미클라이언트들을 돌릴때 로그기록을 보기위한 변수
	*/
	bool _isLogMessage = true;
};
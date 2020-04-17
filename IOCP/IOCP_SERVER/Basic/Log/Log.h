#pragma once
#include <iostream>

using namespace std;

enum class LogIndex : int
{
	WARNING,
	Error,
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

		case LogIndex::Error:
			cout << "Error : " << errNo << " " << errStr << endl;
			exit(1);
			break;

		default:
			break;
		}
	}

private:
};
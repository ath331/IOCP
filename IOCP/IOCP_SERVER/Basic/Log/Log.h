#pragma once
#include <iostream>

using namespace std;

enum class LogIndex : int
{
	WARNING,
	ERROR,
};

class Log
{
public:
	Log(LogIndex index, const char* errStr)
	{
		switch (index)
		{
		case LogIndex::WARNING:
			cout << "Warning : " << errStr << endl;
			break;

		case LogIndex::ERROR:
			cout << "Error : " << errStr << endl;
			exit(1);
			break;

		default:
			break;
		}
	}
private:
};


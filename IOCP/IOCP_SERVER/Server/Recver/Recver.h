#pragma once
#include "OverlappedCustom.h"

class Recver
{
public:
	void PostRecv();
private:
	char _buff[1024];
	Overlapped* _overlapped;
};
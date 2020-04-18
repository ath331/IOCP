#pragma once

#define _CRT_RAND_S  
#include <stdlib.h>

template<typename T>
T GetThreadSafeRandom(int minNum, int numRange)
{
	int uNumber = 0;
	return (T)((unsigned int)((double)uNumber / ((double)UINT_MAX + 1.0) * numRange) + minNum);
}
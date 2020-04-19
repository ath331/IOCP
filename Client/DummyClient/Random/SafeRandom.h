#pragma once
#include <iostream>
#include <random>

class SafeRandom
{
public:
	template <typename T, typename T2>
	T GetRandomNum(T minNum, T2 maxNum)
	{
		if (minNum > maxNum)
		{
			T temp = minNum;
			minNum = maxNum;
			maxNum = temp;
		}

		std::random_device _rd;
		std::mt19937 gen(_rd());

		std::uniform_int_distribution<T> dis(minNum, maxNum);
		return dis(gen);
	}
private:
};


/*
Dummy Client�� ���� ���������� �׽�Ʈ�غ������� ������Ʈ.
���� ����ڵ��� Ȱ���� ���� �� ���ִ�. (�α���, �游���, ������, ä�õ��)
Ŭ���̾�Ʈ�� ������ ������ ���� �������� �ʾҴ�.
*/

#include <iostream>
#include "ThreadManager/ThreadManager.h"

using namespace std;

int main()
{
	cout << "Dummy Client Project" << endl;
	ThreadManager threadManager;
	threadManager.Init();
	threadManager.Start();
}
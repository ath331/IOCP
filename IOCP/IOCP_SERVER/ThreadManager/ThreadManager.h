#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <queue>
#include "packet.h"
#include "Lock.h"

using namespace std;

class ClientManager;
class ThreadManager
{
public:
	void InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager);
	void MakeThread();
private:
	HANDLE _comPort;
	int _maxThreadNum = 0;

	void _MakeIOThreads();
	void _MakeLogicThread();

	static unsigned int WINAPI _RunIOThreadMain(HANDLE completionPortIO);
	static unsigned int WINAPI _RunLogicThreadMain(HANDLE completionPortIO);

	static Lock _packetQueueLock;
	static queue<PacketInfo> _packetQueue;
	static void _pushPacketQueue(SOCKET sock, PacketIndex packetIndex, const char buffer[]);

	static ClientManager* _clientManager;
};


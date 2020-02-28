#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <queue>

#include "packet.h"
#include "Lock.h"

#include "RoomManager/RoomManager.h"

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

	static void SendMessageToClient(int roomNum,const char* msg,bool isEnterMessage = FALSE);

	static ClientManager* _clientManager;

	//static이라 스레드 동기화 이슈가 있겠지만 해당 객체는 singleLogicThread만 접근한다
	static RoomManager _roomManager;
};


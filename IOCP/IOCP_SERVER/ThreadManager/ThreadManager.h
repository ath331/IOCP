#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <queue>

#include "packet.h"
#include "RoomManager/RoomManager.h"
#include <concurrent_queue.h>

using namespace std;

enum class QueueIndex : int
{
	DB,
	NORMAL_QUEUE, //DB관련 패킷이 아닌 패킷들
};

class DB;
class ClientManager;
class RoomManager;
class ThreadManager
{
public:
	void InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager, DB* db);
	void MakeThread();
private:

	HANDLE _comPort;
	int _maxThreadNum = 0;

	void _MakeIOThreads();
	void _MakeLogicThread();
	void _MakeDBThread();

	static unsigned int WINAPI _RunIOThreadMain(void* thisObject);
	static unsigned int WINAPI _RunLogicThreadMain(void* thisObject);
	static unsigned int WINAPI _RunDBThreadMain(void* thisObject);

	concurrency::concurrent_queue<PacketInfo> _packetDBQueue;
	concurrency::concurrent_queue<PacketInfo> _packetQueue;

	void _PushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[]);
	void _SendMessageToClient(int roomNum,const char* msg, bool isSystemMessage = FALSE);

	ClientManager* _clientManager;
	DB* _db;

	RoomManager _roomManager;
};


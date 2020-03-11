#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <queue>

#include "packet.h"
#include "Lock.h"


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

	Lock _packetQueueLock;
	queue<PacketInfo> _packetQueue;
	Lock _packetDBQueueLock;
	queue<PacketInfo> _packetDBQueue;

	void _PushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[]);

	void _SendMessageToClient(int roomNum,const char* msg, bool isSystemMessage = FALSE);

	static ClientManager* _clientManager;
	static DB* _db;

	//static이라 스레드 동기화 이슈가 있겠지만 해당 객체는 singleLogicThread만 접근한다
	static RoomManager _roomManager;
};


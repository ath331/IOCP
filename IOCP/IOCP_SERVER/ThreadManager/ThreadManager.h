#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <vector>
#include <concurrent_queue.h>

#include "packet.h"

using namespace std;

class DB;
class ClientManager;
class Acceptor;
class RoomManager;
class ThreadManager
{
public:
	void InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager, DB* db, Acceptor* accept, RoomManager* roomManager);
	void MakeThread();
	void WaitThread();
private:

	HANDLE _comPort;
	vector<HANDLE> _threadHandleVec;
	int _maxThreadNum = 0;

	void _MakeIOThreads();
	void _MakeLogicThread();
	void _MakeDBThread();

	static unsigned int WINAPI _RunIOThreadMain(void* thisObject);
	static unsigned int WINAPI _RunLogicThreadMain(void* thisObject);
	static unsigned int WINAPI _RunDBThreadMain(void* thisObject);

	concurrency::concurrent_queue<PacketInfo> _packetDBQueue;
	concurrency::concurrent_queue<PacketInfo> _packetQueue;

	void _PushPacketQueue(SOCKET sock, PacketIndex packetIndex, const char buffer[]);
	void _SendMessageToClient(int roomNum,const char* msg, bool isSystemMessage = FALSE);

	//TODO : class °ü¸® ´Ùµë±â
	ClientManager* _clientManager;
	DB* _db;
	Acceptor* _acceptor;

	RoomManager* _roomManager;
};


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

	static unsigned int WINAPI _RunIOThreadMain(void* thisObject); //IO처리 멀티스레드
	static unsigned int WINAPI _RunLogicThreadMain(void* thisObject); //Logic처리 싱글스레드
	static unsigned int WINAPI _RunDBThreadMain(void* thisObject); //DB처리 싱글스레드

	concurrency::concurrent_queue<PacketInfo> _packetDBQueue;
	concurrency::concurrent_queue<PacketInfo> _packetQueue;

	void _SendSystemMessage(int roomNum, const char* name, bool isEnter = TRUE);
	void _SendMessageToClient(SOCKET sock, const char* pckBuf);

	//TODO : class 관리 다듬기
	ClientManager* _clientManager;
	DB* _db;
	Acceptor* _acceptor;

	RoomManager* _roomManager;

	PacketInfo _dbThreadPacket;
	PacketInfo _logicThreadPacket;
};


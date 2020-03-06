#pragma once
#include <WinSock2.h>
#include <windows.h>
#include <queue>

#include "packet.h"
#include "Lock.h"

#include "RoomManager/RoomManager.h"

using namespace std;

enum class QueueIndex : int
{
	DB,
	NORMAL_QUEUE, //DB���� ��Ŷ�� �ƴ� ��Ŷ��
};

class DB;
class ClientManager;
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

	static unsigned int WINAPI _RunIOThreadMain(HANDLE completionPortIO);
	static unsigned int WINAPI _RunLogicThreadMain(HANDLE completionPortIO);
	static unsigned int WINAPI _RunDBThreadMain(HANDLE completionPortIO);

	static Lock _packetDBQueueLock;
	static queue<PacketInfo> _packetDBQueue;

	static Lock _packetQueueLock;
	static queue<PacketInfo> _packetQueue;

	static void _pushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[]);

	static void SendMessageToClient(int roomNum,const char* msg, bool isSystemMessage = FALSE);

	static ClientManager* _clientManager;
	static DB* _db;

	//static�̶� ������ ����ȭ �̽��� �ְ����� �ش� ��ü�� singleLogicThread�� �����Ѵ�
	static RoomManager _roomManager;
};


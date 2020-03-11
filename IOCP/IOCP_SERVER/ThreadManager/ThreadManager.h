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
	NORMAL_QUEUE, //DB���� ��Ŷ�� �ƴ� ��Ŷ��
};

struct LogicStructData
{
	Lock _packetQueueLock;
	queue<PacketInfo> _packetQueue;
};

struct DBStructData
{
	Lock _packetDBQueueLock;
	queue<PacketInfo> _packetDBQueue;
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

	static unsigned int WINAPI _RunIOThreadMain(void* dataInIOThreadMain);
	static unsigned int WINAPI _RunLogicThreadMain(void* logicStructData);
	static unsigned int WINAPI _RunDBThreadMain(void* dbStructDaba);

	LogicStructData logicData;
	DBStructData dbData;

	void _pushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[]);

	static void _SendMessageToClient(int roomNum,const char* msg, bool isSystemMessage = FALSE);

	static ClientManager* _clientManager;
	static DB* _db;

	//static�̶� ������ ����ȭ �̽��� �ְ����� �ش� ��ü�� singleLogicThread�� �����Ѵ�
	static RoomManager _roomManager;
};


#include "ThreadManager.h"
#include "ClientInfo.h"
#include "OverlappedCustom.h"
#include "../Server/ClientManager/ClientManager.h"

#include <process.h>
#include <iostream>

Lock ThreadManager::_packetQueueLock;
queue<PacketInfo> ThreadManager::_packetQueue;
ClientManager* ThreadManager::_clientManager;
RoomManager ThreadManager::_roomManager;

void ThreadManager::InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager)
{
	_maxThreadNum = maxThreadNum;
	_comPort = comPort;
	_clientManager = clientManager;
}

void ThreadManager::MakeThread()
{
	_MakeIOThreads();
	_MakeLogicThread();
}

void ThreadManager::_MakeIOThreads()
{
	for (int i = 0; i < _maxThreadNum - 1; i++) //한개의 쓰레드는 SingleLogicThread로 사용
	{
		_beginthreadex(NULL, 0, _RunIOThreadMain, _comPort, 0, NULL);
	}
}

void ThreadManager::_MakeLogicThread()
{
	_beginthreadex(NULL, 0, _RunLogicThreadMain, NULL, 0, NULL);
}

void ThreadManager::_pushPacketQueue(SOCKET sock, PacketIndex packetIndex, const char buffer[])
{
	LockGuard pushQueueLockGuard(_packetQueueLock);
	PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
	_packetQueue.push(tempPacketInfo);
}


unsigned int WINAPI ThreadManager::_RunIOThreadMain(HANDLE completionPort)
{
	SOCKET sock;
	DWORD bytesTrans;
	ClientInfo* clientInfo;
	Overlapped* ioInfo;
	DWORD flags = 0;

	while (1)
	{
		GetQueuedCompletionStatus(completionPort, &bytesTrans, (PULONG_PTR)&clientInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = clientInfo->clientSock;

		if (ioInfo->rwMode == Overlapped::IO_TYPE::READ)
		{
			std::cout << "message received!\n";
			if (bytesTrans == 0)
			{
				_clientManager->PopClientInfo(clientInfo->clientSock);
				continue;
			}
			else if (bytesTrans < sizeof(PacketHeader)) //PacketHeader size만큼 수신하지 못했으면 추가로 읽기
			{
				int readDataLen = bytesTrans;
				while (1)
				{
					readDataLen += recv(clientInfo->clientSock, &ioInfo->wsaBuf.buf[readDataLen], sizeof(PacketHeader), 0);
					if (readDataLen >= sizeof(PacketHeader))
					{
						break;
					}
				}
			}
			PacketHeader packetHeader;
			memcpy(&packetHeader, &(ioInfo->buffer), sizeof(packetHeader.headerSize));
			//TODO : packetHeader.headerSize가 패킷의 총 길이이므로 추가로 읽어야한다면 recv 구현

			_pushPacketQueue(clientInfo->clientSock, packetHeader.index, ioInfo->buffer);
		}
		else if (ioInfo->rwMode == Overlapped::IO_TYPE::WRITE)
		{
			std::cout << "message sent!\n";
		}
		else
		{
			std::cout << "IO_TYPE is NULL\n";
		}

		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->rwMode = Overlapped::IO_TYPE::READ;
		WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunLogicThreadMain(HANDLE completionPortIO)
{
	while (true)
	{
		Sleep(1);
		LockGuard pushQueueLockGuard(_packetQueueLock); //queue에 패킷을 넣을때와 같은 lock객체를 이용
		if (!_packetQueue.empty())
		{
			PacketInfo packetInfo;
			packetInfo = _packetQueue.front();
			_packetQueue.pop();

			switch (packetInfo.packetIndex)
			{
			case PacketIndex::Login:
			{
				PacketLogin packetLogin;
				memcpy(&packetLogin, packetInfo.packetBuffer, sizeof(PacketLogin));

				ClientInfo clientInfo = { packetInfo.sock };
				memcpy((void*)clientInfo.clientName, packetLogin.name, sizeof(packetLogin.name));
				_clientManager->PushClientInfo(clientInfo);
			}
			break;

			case PacketIndex::MAKE_ROOM:
			{
				PacketMakeRoom packetMakeRoom;
				memcpy(&packetMakeRoom, packetInfo.packetBuffer, sizeof(PacketMakeRoom));
				ClientInfo clientInfo = _clientManager->GetClientInfo(packetInfo.sock);
				_roomManager.MakeRoom(packetMakeRoom.roomName, clientInfo, packetMakeRoom.maxClientCount, packetMakeRoom.isPrivateRoom);
			}
			break;

			default:
				break;
			}
		}
	}
	return 0;
}

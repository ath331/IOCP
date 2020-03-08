#include "ThreadManager.h"
#include "ClientInfo.h"
#include "OverlappedCustom.h"
#include "../Server/ClientManager/ClientManager.h"
#include "DB.h"

#include <process.h>
#include <iostream>

#include <vector>
#include <algorithm>

Lock ThreadManager::_packetQueueLock;
queue<PacketInfo> ThreadManager::_packetQueue;
Lock ThreadManager::_packetDBQueueLock;
queue<PacketInfo> ThreadManager::_packetDBQueue;
ClientManager* ThreadManager::_clientManager;
DB* ThreadManager::_db;
RoomManager ThreadManager::_roomManager;

void ThreadManager::InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager, DB* db)
{
	_maxThreadNum = maxThreadNum;
	_comPort = comPort;
	_clientManager = clientManager;
	_db = db;
}

void ThreadManager::MakeThread()
{
	_MakeIOThreads();
	_MakeLogicThread();
	_MakeDBThread();
}

void ThreadManager::_MakeIOThreads()
{
	for (int i = 0; i < _maxThreadNum - 2; i++)
	{
		_beginthreadex(NULL, 0, _RunIOThreadMain, _comPort, 0, NULL);
	}
}

void ThreadManager::_MakeLogicThread()
{
	_beginthreadex(NULL, 0, _RunLogicThreadMain, NULL, 0, NULL);
}

void ThreadManager::_MakeDBThread()
{
	_beginthreadex(NULL, 0, _RunDBThreadMain, NULL, 0, NULL);
}

void ThreadManager::_pushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[])
{
	if (queueIndex == QueueIndex::NORMAL_QUEUE)
	{
		LockGuard pushQueueLockGuard(_packetQueueLock);
		PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
		_packetQueue.push(tempPacketInfo);
	}
	else if (queueIndex == QueueIndex::DB)
	{
		LockGuard pushDBQueueLockGuard(_packetDBQueueLock);
		PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
		_packetDBQueue.push(tempPacketInfo);
	}
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

			if (packetHeader.index > PacketIndex::DB_INDEX)
			{
				_pushPacketQueue(QueueIndex::DB, clientInfo->clientSock, packetHeader.index, ioInfo->buffer);
			}
			else
				_pushPacketQueue(QueueIndex::NORMAL_QUEUE, clientInfo->clientSock, packetHeader.index, ioInfo->buffer);

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
				/*case PacketIndex::Login:
				{
					PacketLogin packetLogin;
					memcpy(&packetLogin, packetInfo.packetBuffer, sizeof(PacketLogin));

					ClientInfo* clientInfo = new ClientInfo(packetInfo.sock);
					memcpy((void*)clientInfo->clientName, packetLogin.name, sizeof(packetLogin.name));
					_clientManager->PushClientInfo(clientInfo);
				}
				break;*/

			case PacketIndex::MAKE_ROOM:
			{
				//TODO : 방만들기와 방접속 분리하기
				PacketMakeRoom packetMakeRoom;
				memcpy(&packetMakeRoom, packetInfo.packetBuffer, sizeof(PacketMakeRoom));
				ClientInfo* clientInfo = _clientManager->GetClientInfo(packetInfo.sock);
				_roomManager.MakeRoom(packetMakeRoom.roomName, clientInfo, packetMakeRoom.maxClientCount, packetMakeRoom.isPrivateRoom);

				RES_PacketMakeRoom resPacketMakeRoom;
				resPacketMakeRoom.roomNum = _roomManager.GetRoomCount() - 1;
				//clientInfo->roomNum.push_back(resPacketMakeRoom.roomNum);
				clientInfo->roomNum = resPacketMakeRoom.roomNum;
				send(packetInfo.sock, (const char*)&resPacketMakeRoom, resPacketMakeRoom.header.headerSize, 0);
			}
			break;

			case PacketIndex::ROOM_LIST:
			{
				RES_PacketRoomList resPacketRoomList;
				resPacketRoomList.maxRoomCount = _roomManager.GetRoomVecSize();
				if (resPacketRoomList.maxRoomCount != 0) //만든 방이 하나라도 있을때
				{
					if (resPacketRoomList.maxRoomCount > MAX_ROOM_COUNT)
						resPacketRoomList.maxRoomCount = MAX_ROOM_COUNT;
					for (int i = 0; i < resPacketRoomList.maxRoomCount; i++)
					{
						resPacketRoomList.roomInfoList[i].roomNum = _roomManager.GetRoomInfoByCountNum(i).GetRoomNum();
						memcpy((void*)&resPacketRoomList.roomInfoList[i].roomName, _roomManager.GetRoomInfoByCountNum(i).GetRoomName().c_str(), strlen(_roomManager.GetRoomInfoByCountNum(i).GetRoomName().c_str()));
						resPacketRoomList.roomInfoList[i].maxClientInRoom = _roomManager.GetRoomInfoByCountNum(i).GetMaxClientCount();
						resPacketRoomList.roomInfoList[i].curClientNum = _roomManager.GetRoomInfoByCountNum(i).clientInfoVec.size();
					}
					send(packetInfo.sock, (const char*)&resPacketRoomList, resPacketRoomList.header.headerSize, 0);
				}
			}
			break;

			case PacketIndex::ENTER_ROOM:
			{
				PacketEnterRoom packetEnterRoom;
				memcpy(&packetEnterRoom, packetInfo.packetBuffer, sizeof(PacketEnterRoom));
				ClientInfo* clientInfo = _clientManager->GetClientInfo(packetInfo.sock);
				//clientInfo->roomNum.push_back(packetEnterRoom.roomNum);
				clientInfo->roomNum = packetEnterRoom.roomNum;
				_roomManager.EnterRoom(packetEnterRoom.roomNum, clientInfo);

				//TODO : Message 관리 class 만들기
				string enterMessage = "[SYSTEM] ";
				enterMessage += clientInfo->clientName;
				enterMessage += "님이 접속했습니다.";
				SendMessageToClient(packetEnterRoom.roomNum, enterMessage.c_str(), TRUE);
			}
			break;

			case PacketIndex::CLOSE_ROOM:
			{
				PacketCloseRoom packetCloseRoom;
				memcpy(&packetCloseRoom, packetInfo.packetBuffer, sizeof(PacketCloseRoom));
				ClientInfo* clientInfo = _clientManager->GetClientInfo(packetInfo.sock);
				clientInfo->OutRoom(packetCloseRoom.roomNum);
				_roomManager.OutClientInRoom(packetInfo.sock, packetCloseRoom.roomNum);

				//TODO : Message 관리 class 만들기
				string enterMessage = "[SYSTEM] ";
				enterMessage += clientInfo->clientName;
				enterMessage += "님이 나갔습니다.";
				SendMessageToClient(packetCloseRoom.roomNum, enterMessage.c_str(), TRUE);
			}
			break;

			case PacketIndex::SEND_MESSAGE:
			{
				PacketSendMessage packetSendMessage;
				memcpy(&packetSendMessage, packetInfo.packetBuffer, sizeof(PacketSendMessage));
				SendMessageToClient(packetSendMessage.roomNum, packetSendMessage.buffer);
			}
			break;

			default:
				break;
			}
		}
	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunDBThreadMain(HANDLE completionPortIO)
{
	while (true)
	{
		Sleep(1);
		LockGuard pushDBQueueLockGuard(_packetDBQueueLock);
		if (!_packetDBQueue.empty())
		{
			PacketInfo packetInfo;
			packetInfo = _packetDBQueue.front();
			_packetDBQueue.pop();

			switch (packetInfo.packetIndex)
			{
			case PacketIndex::Login:
			{
				PacketLogin packetLogin;
				memcpy(&packetLogin, packetInfo.packetBuffer, sizeof(PacketLogin));
				if (_db->CheckIdPw(packetLogin.id, packetLogin.pw))
				{
					string name = _db->GetName(packetLogin.id);

					ClientInfo* clientInfo = new ClientInfo(packetInfo.sock);
					memcpy((void*)clientInfo->clientName, (const void*)name.c_str(), sizeof(name.c_str()));
					_clientManager->PushClientInfo(clientInfo);

					_db->UpdateData(UpdataType::SOCK, packetLogin.id, packetLogin.name, packetInfo.sock);

					memcpy((void*)&packetLogin.name, (const void*)name.c_str(), sizeof(name.c_str()));
					packetLogin.isSuccessIdCheck = TRUE;
					send(packetInfo.sock, (const char*)&packetLogin, packetLogin.header.headerSize, 0);
				}
			}
			break;

			case PacketIndex::MAKE_CLIENT_ID_INFO:
			{
				PacketClientIdInfo packetClientIdInfo;
				memcpy(&packetClientIdInfo, packetInfo.packetBuffer, sizeof(PacketClientIdInfo));

				if (packetClientIdInfo.isChangeName == FALSE)
				{
					PacketDBInsertData packetDbInsertData;
					if (_db->InsertData(packetClientIdInfo.id, packetClientIdInfo.pw, packetClientIdInfo.name))
					{
						//DB에 데이터등록이 성공할 경우
						packetDbInsertData.isSuccessInsertData = TRUE;
					}
					send(packetInfo.sock, (const char*)&packetDbInsertData, packetDbInsertData.header.headerSize, 0);
					break;
				}
				else if (packetClientIdInfo.isChangeName == TRUE)
				{
					_db->UpdateData(UpdataType::NAME, "tempId", packetClientIdInfo.name, packetInfo.sock);
					break;
				}
			}

			default:
				break;
			}
		}
	}
}


void ThreadManager::SendMessageToClient(int roomNum, const char* msg, bool isSystemMessage)
{
	Room room = _roomManager.GetRoomInfoByRoomNum(roomNum);
	int clientCount = room.clientInfoVec.size();

	for (int i = 0; i < clientCount; i++)
	{
		SOCKET sock = _roomManager.GetRoomInfoByRoomNum(roomNum).clientInfoVec[i]->clientSock;
		send(sock, msg, strlen(msg), 0);
	}
}

#include "ThreadManager.h"
#include "ClientInfo.h"
#include "OverlappedCustom.h"
#include "../Server/ClientManager/ClientManager.h"
#include "DB.h"

#include <process.h>
#include <iostream>

#include <algorithm>

#include "../Server/Session/TcpSession.h"
#include "../Server/Session/Acceptor/Acceptor.h"

void ThreadManager::InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager, DB* db, Acceptor* acceptor)
{
	_maxThreadNum = maxThreadNum;
	_comPort = comPort;
	_clientManager = clientManager;
	_db = db;
	_acceptor = acceptor;
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
		HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, _RunIOThreadMain, this, 0, NULL);
		_threadHandleVec.push_back(threadHandle);
	}
}

void ThreadManager::_MakeLogicThread()
{
	HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, _RunLogicThreadMain, this, 0, NULL);
	_threadHandleVec.push_back(threadHandle);
}

void ThreadManager::_MakeDBThread()
{
	HANDLE threadHandle = (HANDLE)_beginthreadex(NULL, 0, _RunDBThreadMain, this, 0, NULL);
	_threadHandleVec.push_back(threadHandle);
}

void ThreadManager::WaitThread()
{
	if (_threadHandleVec.empty() == false)
		WaitForMultipleObjects(_threadHandleVec.size(), &_threadHandleVec[0], true, INFINITE);
}


void ThreadManager::_PushPacketQueue(SOCKET sock, PacketIndex packetIndex, const char buffer[])
{
	PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
	if (packetIndex < PacketIndex::DB_INDEX)
	{
		_packetQueue.push(tempPacketInfo);
	}
	else if (packetIndex > PacketIndex::DB_INDEX)
	{
		_packetDBQueue.push(tempPacketInfo);
	}
}


unsigned int WINAPI ThreadManager::_RunIOThreadMain(void* _thisObject)
{
	ThreadManager* thisObject = (ThreadManager*)_thisObject;
	DWORD bytesTrans = 0;
	Overlapped* ioInfo = nullptr;
	SOCKET sock = 0; //CompletionKey

	while (1)
	{
		GetQueuedCompletionStatus(thisObject->_comPort, &bytesTrans, (PULONG_PTR)&sock, (LPOVERLAPPED*)&ioInfo, INFINITE);

		if (ioInfo->ioType == Overlapped::IO_TYPE::ACCEPT)
		{
			SOCKET sock = ioInfo->sock; //접속한 clientSock
			TcpSession* session = new TcpSession(thisObject->_comPort, sock, &thisObject->_packetQueue, &thisObject->_packetDBQueue);
			thisObject->_clientManager->clientSessionMap.insert(make_pair(sock, session));
			session->PostRecv();

			thisObject->_acceptor->AcceptClient();
		}

		else if (ioInfo->ioType == Overlapped::IO_TYPE::RECV)
		{
			if (bytesTrans == 0)
			{
				thisObject->_clientManager->CloseClient(sock);
				continue;
			}
			thisObject->_clientManager->clientSessionMap.find(sock)->second->CheckPcketSize(bytesTrans);
		}

		else if (ioInfo->ioType == Overlapped::IO_TYPE::SEND)
		{
			thisObject->_clientManager->clientSessionMap.find(sock)->second->isSending = false;
		}

	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunLogicThreadMain(void* _thisObject)
{
	ThreadManager* thisObject = (ThreadManager*)_thisObject;
	while (true)
	{
		Sleep(1);
		if (!(thisObject->_packetQueue.empty()))
		{
			PacketInfo packetInfo;
			thisObject->_packetQueue.try_pop(packetInfo);

			switch (packetInfo.packetIndex)
			{
			case PacketIndex::MAKE_ROOM:
			{
				//TODO : 방만들기와 방접속 분리하기
				PacketMakeRoom packetMakeRoom;
				memcpy(&packetMakeRoom, packetInfo.packetBuffer, sizeof(PacketMakeRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				thisObject->_roomManager.MakeRoom(packetMakeRoom.roomName, clientInfo, packetMakeRoom.maxClientCount, packetMakeRoom.isPrivateRoom);

				RES_PacketMakeRoom resPacketMakeRoom;
				resPacketMakeRoom.roomNum = thisObject->_roomManager.GetRoomCount() - 1;
				clientInfo->roomNum = resPacketMakeRoom.roomNum;

				packetInfo.packetBuffer = (const char*)&resPacketMakeRoom;
				thisObject->_clientManager->clientSessionMap.find(packetInfo.sock)->second->PushSendVec(packetInfo, sizeof(RES_PacketMakeRoom));
			}
			break;

			case PacketIndex::ROOM_LIST:
			{
				RES_PacketRoomList resPacketRoomList;
				resPacketRoomList.maxRoomCount = thisObject->_roomManager.GetRoomVecSize();
				if (resPacketRoomList.maxRoomCount != 0) //만든 방이 하나라도 있을때
				{
					if (resPacketRoomList.maxRoomCount > MAX_ROOM_COUNT)
						resPacketRoomList.maxRoomCount = MAX_ROOM_COUNT;
					for (int i = 0; i < resPacketRoomList.maxRoomCount; i++)
					{
						resPacketRoomList.roomInfoList[i].roomNum = thisObject->_roomManager.GetRoomInfoByCountNum(i).GetRoomNum();
						memcpy((void*)&resPacketRoomList.roomInfoList[i].roomName, thisObject->_roomManager.GetRoomInfoByCountNum(i).GetRoomName().c_str(), strlen(thisObject->_roomManager.GetRoomInfoByCountNum(i).GetRoomName().c_str()));
						resPacketRoomList.roomInfoList[i].maxClientInRoom = thisObject->_roomManager.GetRoomInfoByCountNum(i).GetMaxClientCount();
						resPacketRoomList.roomInfoList[i].curClientNum = thisObject->_roomManager.GetRoomInfoByCountNum(i).clientInfoVec.size();
					}
					packetInfo.packetBuffer = (const char*)&resPacketRoomList;
					thisObject->_clientManager->clientSessionMap.find(packetInfo.sock)->second->PushSendVec(packetInfo, sizeof(RES_PacketRoomList));
				}
			}
			break;

			case PacketIndex::ENTER_ROOM:
			{
				PacketEnterRoom packetEnterRoom;
				memcpy(&packetEnterRoom, packetInfo.packetBuffer, sizeof(PacketEnterRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				clientInfo->roomNum = packetEnterRoom.roomNum;
				thisObject->_roomManager.EnterRoom(packetEnterRoom.roomNum, clientInfo);

				//TODO : Message 관리 class 만들기
				string enterMessage = "[SYSTEM] ";
				enterMessage += clientInfo->clientName;
				enterMessage += "님이 접속했습니다.";
				thisObject->_SendMessageToClient(packetEnterRoom.roomNum, enterMessage.c_str(), TRUE);
			}
			break;

			case PacketIndex::CLOSE_ROOM:
			{
				PacketCloseRoom packetCloseRoom;
				memcpy(&packetCloseRoom, packetInfo.packetBuffer, sizeof(PacketCloseRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				clientInfo->OutRoom(packetCloseRoom.roomNum);
				if ((thisObject->_roomManager.OutClientInRoom(packetInfo.sock, packetCloseRoom.roomNum)))
				{
					//TODO : Message 관리 class 만들기
					string enterMessage = "[SYSTEM] ";
					enterMessage += clientInfo->clientName;
					enterMessage += "님이 나갔습니다.";
					thisObject->_SendMessageToClient(packetCloseRoom.roomNum, enterMessage.c_str(), TRUE);
				}
			}
			break;

			case PacketIndex::SEND_MESSAGE:
			{
				PacketSendMessage packetSendMessage;
				memmove(&packetSendMessage, packetInfo.packetBuffer, sizeof(PacketSendMessage));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				packetSendMessage.roomNum = clientInfo->roomNum;
				thisObject->_SendMessageToClient(packetSendMessage.roomNum, packetSendMessage.buffer);
			}
			break;

			default:
				break;
			}
		}
	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunDBThreadMain(void* _thisObject)
{
	ThreadManager* thisObject = (ThreadManager*)_thisObject;
	while (true)
	{
		Sleep(1);
		if (!(thisObject->_packetDBQueue.empty()))
		{
			PacketInfo packetInfo;
			thisObject->_packetDBQueue.try_pop(packetInfo);

			switch (packetInfo.packetIndex)
			{
			case PacketIndex::Login:
			{
				PacketLogin packetLogin;
				memcpy(&packetLogin, packetInfo.packetBuffer, sizeof(PacketLogin));
				if (thisObject->_db->CheckIdPw(packetLogin.id, packetLogin.pw))
				{
					string name = thisObject->_db->GetName(packetLogin.id);

					ClientInfo* clientInfo = new ClientInfo(packetInfo.sock);
					memcpy((void*)clientInfo->clientName, (const void*)name.c_str(), sizeof(name.c_str()));
					thisObject->_clientManager->PushClientInfo(clientInfo);

					thisObject->_db->UpdateData(UpdataType::SOCK, packetLogin.id, packetLogin.name, packetInfo.sock);

					memcpy((void*)&packetLogin.name, (const void*)name.c_str(), sizeof(name.c_str()));
					packetLogin.isSuccessIdCheck = TRUE;
					packetInfo.packetBuffer = (const char*)&packetLogin;
					thisObject->_clientManager->clientSessionMap.find(packetInfo.sock)->second->PushSendVec(packetInfo, sizeof(PacketLogin));
				}
			}
			break;

			case PacketIndex::MAKE_CLIENT_ID_INFO:
			{
				//PacketClientIdInfo packetClientIdInfo;
				//memcpy(&packetClientIdInfo, packetInfo.packetBuffer, sizeof(PacketClientIdInfo));

				//if (packetClientIdInfo.isChangeName == FALSE)
				//{
				//	PacketDBInsertData packetDbInsertData;
				//	if (thisObject->_db->InsertData(packetClientIdInfo.id, packetClientIdInfo.pw, packetClientIdInfo.name))
				//	{
				//		//DB에 데이터등록이 성공할 경우
				//		packetDbInsertData.isSuccessInsertData = TRUE;
				//	}
				//	send(packetInfo.sock, (const char*)&packetDbInsertData, packetDbInsertData.header.headerSize, 0);
				//	break;
				//}
				//else if (packetClientIdInfo.isChangeName == TRUE)
				//{
				//	thisObject->_db->UpdateData(UpdataType::NAME, "tempId", packetClientIdInfo.name, packetInfo.sock);
				//	break;
				//}
			}

			default:
				break;
			}
		}
	}
}


void ThreadManager::_SendMessageToClient(int roomNum, const char* msg, bool isSystemMessage)
{
	Room room = _roomManager.GetRoomInfoByRoomNum(roomNum);
	int clientCount = room.clientInfoVec.size();

	for (int i = 0; i < clientCount; i++)
	{
		SOCKET sock = _roomManager.GetRoomInfoByRoomNum(roomNum).clientInfoVec[i]->clientSock;
		send(sock, msg, strlen(msg), 0);
	}
}

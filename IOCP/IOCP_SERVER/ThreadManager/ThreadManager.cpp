#include "ThreadManager.h"
#include "../IOCP_SERVER/Server/ClientManager/ClientInfo.h"
#include "OverlappedCustom.h"
#include "DB.h"

#include <process.h>
#include <iostream>
#include <algorithm>

#include "../Server/Acceptor/Acceptor.h"
#include "../Server/Session/TcpSession.h"
#include "../Server/ClientManager/ClientManager.h"
#include "../Server/RoomManager/RoomManager.h"

void ThreadManager::InitThreadManager(int maxThreadNum, HANDLE comPort, ClientManager* clientManager, DB* db, Acceptor* acceptor, RoomManager* roomManager)
{
	_maxThreadNum = maxThreadNum;
	_comPort = comPort;
	_clientManager = clientManager;
	_db = db;
	_acceptor = acceptor;
	_roomManager = roomManager;
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
		WaitForMultipleObjects(static_cast<DWORD>(_threadHandleVec.size()), &_threadHandleVec[0], true, INFINITE);
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
			thisObject->_clientManager->clientSessionMap.find(sock)->second->OnRecvForIocp(bytesTrans);
		}

		else if (ioInfo->ioType == Overlapped::IO_TYPE::SEND)
		{
			thisObject->_clientManager->clientSessionMap.find(sock)->second->OnSendForIocp();
		}

	}
	return 0;
}

unsigned int WINAPI ThreadManager::_RunLogicThreadMain(void* _thisObject)
{
	ThreadManager* thisObject = (ThreadManager*)_thisObject;
	PacketInfo packetInfo;
	TcpSession* clientSession = nullptr;
	while (true)
	{
		Sleep(1);
		if (thisObject->_packetQueue.try_pop(packetInfo))
		{
			clientSession = thisObject->_clientManager->clientSessionMap.find(packetInfo.sock)->second;
			switch (packetInfo.packetIndex)
			{
			case PacketIndex::MAKE_ROOM:
			{
				PacketMakeRoom packetMakeRoom;
				memcpy(&packetMakeRoom, packetInfo.packetBuffer, sizeof(PacketMakeRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				thisObject->_roomManager->MakeRoom(packetMakeRoom.roomName, clientInfo, packetMakeRoom.maxClientCount, packetMakeRoom.isPrivateRoom);

				RES_PacketMakeRoom resPacketMakeRoom;
				resPacketMakeRoom.roomNum = thisObject->_roomManager->GetRoomCount() - 1;
				clientInfo->roomNum = resPacketMakeRoom.roomNum;

				packetInfo.packetBuffer = (const char*)&resPacketMakeRoom;
				clientSession->PushSendVec(packetInfo, sizeof(RES_PacketMakeRoom));
			}
			break;

			case PacketIndex::ROOM_LIST:
			{
				RES_PacketRoomList resPacketRoomList;
				resPacketRoomList.maxRoomCount = thisObject->_roomManager->GetRoomVecSize();
				if (resPacketRoomList.maxRoomCount > 0) //만든 방이 하나라도 있을때
				{
					thisObject->_roomManager->SettingRoomList(resPacketRoomList);

					packetInfo.packetBuffer = (const char*)&resPacketRoomList;
					clientSession->PushSendVec(packetInfo, sizeof(RES_PacketRoomList));
				}
			}
			break;

			case PacketIndex::ENTER_ROOM:
			{
				PacketEnterRoom packetEnterRoom;
				memcpy(&packetEnterRoom, packetInfo.packetBuffer, sizeof(PacketEnterRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				clientInfo->roomNum = packetEnterRoom.roomNum;
				thisObject->_roomManager->EnterRoom(packetEnterRoom.roomNum, clientInfo);

				string enterMessage = clientInfo->clientName;
				thisObject->_SendSystemMessage(packetEnterRoom.roomNum, enterMessage.c_str(), TRUE);
			}
			break;

			case PacketIndex::CLOSE_ROOM:
			{
				PacketCloseRoom packetCloseRoom;
				memcpy(&packetCloseRoom, packetInfo.packetBuffer, sizeof(PacketCloseRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				clientInfo->OutRoom(packetCloseRoom.roomNum);
				if ((thisObject->_roomManager->OutClientInRoom(packetInfo.sock, packetCloseRoom.roomNum)))
				{
					string enterMessage = clientInfo->clientName;
					thisObject->_SendSystemMessage(packetCloseRoom.roomNum, enterMessage.c_str(), FALSE);
				}
			}
			break;

			case PacketIndex::SEND_MESSAGE:
			{
				thisObject->_SendMessageToClient(packetInfo.sock, packetInfo.packetBuffer);
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
	PacketInfo packetInfo;
	TcpSession* clientSession = nullptr;
	while (true)
	{
		Sleep(1);
		if (thisObject->_packetDBQueue.try_pop(packetInfo))
		{
			clientSession = thisObject->_clientManager->clientSessionMap.find(packetInfo.sock)->second;
			switch (packetInfo.packetIndex)
			{
			case PacketIndex::Login:
			{
				PacketLogin packetLogin;
				memcpy(&packetLogin, packetInfo.packetBuffer, sizeof(PacketLogin));
				if (thisObject->_db->CheckIdPw(packetLogin.id, packetLogin.pw))
				{
					string name = thisObject->_db->GetName(packetLogin.id);

					thisObject->_clientManager->PushClientInfo(packetInfo.sock, name);

					thisObject->_db->UpdateData(UpdataType::SOCK, packetLogin.id, packetLogin.name, static_cast<int>(packetInfo.sock));

					memcpy((void*)&packetLogin.name, (const void*)name.c_str(), sizeof(name.c_str()));
					packetLogin.isSuccessIdCheck = TRUE;
					packetInfo.packetBuffer = (const char*)&packetLogin;
					clientSession->PushSendVec(packetInfo, sizeof(PacketLogin));
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
					if (thisObject->_db->InsertData(packetClientIdInfo.id, packetClientIdInfo.pw, packetClientIdInfo.name))
					{
						//DB에 데이터등록이 성공할 경우
						packetDbInsertData.isSuccessInsertData = TRUE;
					}
					clientSession->PushSendVec(packetInfo, sizeof(PacketLogin));

					break;
				}
				else if (packetClientIdInfo.isChangeName == TRUE)
				{
					thisObject->_db->UpdateData(UpdataType::NAME, "tempId", packetClientIdInfo.name, static_cast<int>(packetInfo.sock));
					break;
				}
			}

			default:
				break;
			}
		}
	}
}

void ThreadManager::_SendMessageToClient(SOCKET sock, const char* pckBuf)
{
	PacketSendMessage packetSendMessage;
	memmove(&packetSendMessage, pckBuf, sizeof(PacketSendMessage));

	ClientInfo* clientInfo = _clientManager->GetClientInfo(sock);
	int roomNum = clientInfo->roomNum;

	Room room = _roomManager->GetRoomInfoByRoomNum(roomNum);
	int clientCount = static_cast<int>(room.clientInfoVec.size());

	PacketInfo packetInfo;
	packetInfo.packetBuffer = packetSendMessage.buffer;
	for (int i = 0; i < clientCount; i++)
	{
		SOCKET sock = _roomManager->GetRoomInfoByRoomNum(roomNum).clientInfoVec[i]->clientSock;
		_clientManager->clientSessionMap.find(sock)->second->PushSendVec(packetInfo, sizeof(PacketSendMessage));
	}
}


void ThreadManager::_SendSystemMessage(int roomNum, const char* name, bool isEnter)
{
	Room room = _roomManager->GetRoomInfoByRoomNum(roomNum);
	int clientCount = static_cast<int>(room.clientInfoVec.size());

	string tempSystempMsg = "[SYSTEM] ";
	tempSystempMsg += name;

	if (isEnter)
	{
		tempSystempMsg += " 님이 접속 했습니다.";
	}
	else
	{
		tempSystempMsg += " 님이 퇴장 했습니다.";
	}

	PacketInfo packetInfo;
	packetInfo.packetBuffer = tempSystempMsg.c_str();
	for (int i = 0; i < clientCount; i++)
	{
		SOCKET sock = _roomManager->GetRoomInfoByRoomNum(roomNum).clientInfoVec[i]->clientSock;
		_clientManager->clientSessionMap.find(sock)->second->PushSendVec(packetInfo, sizeof(tempSystempMsg));
	}
}

#include "ThreadManager.h"
#include "ClientInfo.h"
#include "OverlappedCustom.h"
#include "../Server/ClientManager/ClientManager.h"
#include "DB.h"

#include <process.h>
#include <iostream>

#include <algorithm>

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
		WaitForMultipleObjects(_threadHandleVec.size(),&_threadHandleVec[0],true,INFINITE);
}


void ThreadManager::_PushPacketQueue(QueueIndex queueIndex, SOCKET sock, PacketIndex packetIndex, const char buffer[])
{
	if (queueIndex == QueueIndex::NORMAL_QUEUE)
	{
		PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
		_packetQueue.push(tempPacketInfo);
	}
	else if (queueIndex == QueueIndex::DB)
	{
		PacketInfo tempPacketInfo = { sock, packetIndex, buffer };
		_packetDBQueue.push(tempPacketInfo);
	}
}


unsigned int WINAPI ThreadManager::_RunIOThreadMain(void* _thisObject)
{
	ThreadManager* thisObject = (ThreadManager*)_thisObject;
	SOCKET sock;
	DWORD bytesTrans;
	ClientInfo* clientInfo;
	Overlapped* ioInfo;
	DWORD flags = 0;

	while (1)
	{
		GetQueuedCompletionStatus(thisObject->_comPort, &bytesTrans, (PULONG_PTR)&clientInfo, (LPOVERLAPPED*)&ioInfo, INFINITE);
		sock = clientInfo->clientSock;

		if (ioInfo->ioType == Overlapped::IO_TYPE::ACCEPT)
		{
			CreateIoCompletionPort((HANDLE)sock, thisObject->_comPort, (ULONG_PTR)clientInfo, 0);
		}

		if (ioInfo->ioType == Overlapped::IO_TYPE::RECV)
		{
			if (bytesTrans == 0)
			{
				thisObject->_clientManager->PopClientInfo(clientInfo->clientSock);
				continue;
			}
			else if (bytesTrans < sizeof(PacketHeader)) //PacketHeader size��ŭ �������� �������� �߰��� �б�
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
			//TODO : packetHeader.headerSize�� ��Ŷ�� �� �����̹Ƿ� �߰��� �о���Ѵٸ� recv ����

			if (packetHeader.index > PacketIndex::DB_INDEX)
			{
				thisObject->_PushPacketQueue(QueueIndex::DB, clientInfo->clientSock, packetHeader.index, ioInfo->buffer);
			}
			else
				thisObject->_PushPacketQueue(QueueIndex::NORMAL_QUEUE, clientInfo->clientSock, packetHeader.index, ioInfo->buffer);

		}
		else if (ioInfo->ioType == Overlapped::IO_TYPE::SEND)
		{
			std::cout << "message sent!\n";
		}
		else
		{
			std::cout << "IO_TYPE is NULL\n";
		}

		ioInfo->wsaBuf.len = BUF_SIZE;
		ioInfo->ioType = Overlapped::IO_TYPE::RECV;
		WSARecv(sock, &(ioInfo->wsaBuf), 1, NULL, &flags, &(ioInfo->overlapped), NULL);
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
				//TODO : �游���� ������ �и��ϱ�
				PacketMakeRoom packetMakeRoom;
				memcpy(&packetMakeRoom, packetInfo.packetBuffer, sizeof(PacketMakeRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				thisObject->_roomManager.MakeRoom(packetMakeRoom.roomName, clientInfo, packetMakeRoom.maxClientCount, packetMakeRoom.isPrivateRoom);

				RES_PacketMakeRoom resPacketMakeRoom;
				resPacketMakeRoom.roomNum = thisObject->_roomManager.GetRoomCount() - 1;
				//clientInfo->roomNum.push_back(resPacketMakeRoom.roomNum);
				clientInfo->roomNum = resPacketMakeRoom.roomNum;
				send(packetInfo.sock, (const char*)&resPacketMakeRoom, resPacketMakeRoom.header.headerSize, 0);
			}
			break;

			case PacketIndex::ROOM_LIST:
			{
				RES_PacketRoomList resPacketRoomList;
				resPacketRoomList.maxRoomCount = thisObject->_roomManager.GetRoomVecSize();
				if (resPacketRoomList.maxRoomCount != 0) //���� ���� �ϳ��� ������
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
					send(packetInfo.sock, (const char*)&resPacketRoomList, resPacketRoomList.header.headerSize, 0);
				}
			}
			break;

			case PacketIndex::ENTER_ROOM:
			{
				PacketEnterRoom packetEnterRoom;
				memcpy(&packetEnterRoom, packetInfo.packetBuffer, sizeof(PacketEnterRoom));
				ClientInfo* clientInfo = thisObject->_clientManager->GetClientInfo(packetInfo.sock);
				//clientInfo->roomNum.push_back(packetEnterRoom.roomNum);
				clientInfo->roomNum = packetEnterRoom.roomNum;
				thisObject->_roomManager.EnterRoom(packetEnterRoom.roomNum, clientInfo);

				//TODO : Message ���� class �����
				string enterMessage = "[SYSTEM] ";
				enterMessage += clientInfo->clientName;
				enterMessage += "���� �����߽��ϴ�.";
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
					//TODO : Message ���� class �����
					string enterMessage = "[SYSTEM] ";
					enterMessage += clientInfo->clientName;
					enterMessage += "���� �������ϴ�.";
					thisObject->_SendMessageToClient(packetCloseRoom.roomNum, enterMessage.c_str(), TRUE);
				}
			}
			break;

			case PacketIndex::SEND_MESSAGE:
			{
				PacketSendMessage packetSendMessage;
				memcpy(&packetSendMessage, packetInfo.packetBuffer, sizeof(PacketSendMessage));
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
					if (thisObject->_db->InsertData(packetClientIdInfo.id, packetClientIdInfo.pw, packetClientIdInfo.name))
					{
						//DB�� �����͵���� ������ ���
						packetDbInsertData.isSuccessInsertData = TRUE;
					}
					send(packetInfo.sock, (const char*)&packetDbInsertData, packetDbInsertData.header.headerSize, 0);
					break;
				}
				else if (packetClientIdInfo.isChangeName == TRUE)
				{
					thisObject->_db->UpdateData(UpdataType::NAME, "tempId", packetClientIdInfo.name, packetInfo.sock);
					break;
				}
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

#include "TestClient.h"
#include "ClientLogic.h"
#include "packet.h"
#include "../DummyClient/ThreadSafeRandom.h"

TestClient::TestClient()
{
	_clientLogic = new ClientLogic;
	_clientLogic->Init("127.0.0.1", 9999);
	_clientLogic->Connect();
}

void TestClient::RunTestClient()
{
	_Login();
	while (true)
	{
		int num = GetThreadSafeRandom<int>(1, 2);
		if (num == 1)
			_MakeRoom();
		else
			_GetRoomList();
	}
}

void TestClient::_Login()
{
	if (!_isLogin)
	{
		PacketLogin packetLogin;
		memcpy((void*)&packetLogin.id, "a", sizeof("a"));
		memcpy((void*)&packetLogin.pw, "1234", sizeof("1234"));
		_clientLogic->SendPacket(PacketIndex::Login, (const char*)&packetLogin);

		memcpy((void*)&packetLogin, &_clientLogic->buf, sizeof(PacketLogin));
		if (packetLogin.isSuccessIdCheck)
			_isLogin = true;
	}
}

void TestClient::_MakeRoom()
{
	if (_isLogin && !_isEnterRoom)
	{
		PacketMakeRoom packetMakeRoom;
		memcpy((void*)&packetMakeRoom.roomName, "TestRoom", sizeof("TestRoom"));
		_clientLogic->SendPacket(PacketIndex::MAKE_ROOM, (const char*)&packetMakeRoom);

		RES_PacketMakeRoom resPacketMakeRoom;
		memcpy((void*)&resPacketMakeRoom, &_clientLogic->buf, sizeof(RES_PacketMakeRoom));
		if (resPacketMakeRoom.roomNum > -1)
		{
			_EnterRoom(resPacketMakeRoom.roomNum);
			_isEnterRoom = true;
		}
	}
}

void TestClient::_SendMessage()
{
	if (_isLogin && !_isEnterRoom)
	{
		PacketSendMessage packetSendMessge;
		memcpy((void*)&packetSendMessge.buffer, "TestMessage", sizeof("TestMessage"));
		_clientLogic->SendPacket(PacketIndex::SEND_MESSAGE, (const char*)&packetSendMessge);
	}
}

void TestClient::_GetRoomList()
{
	if (_isLogin && !_isEnterRoom)
	{
		PacketRoomList packetRoomList;
		_clientLogic->SendPacket(PacketIndex::MAKE_ROOM, (const char*)&packetRoomList);

		RES_PacketRoomList resPacketRoomList;
		_clientLogic->RecvPacket(sizeof(RES_PacketRoomList));
		memcpy((void*)&resPacketRoomList, &_clientLogic->buf, sizeof(RES_PacketRoomList));

		int roomCount = resPacketRoomList.maxRoomCount;
		if (roomCount > 0)
		{
			unsigned int roomIndex = GetThreadSafeRandom<unsigned int>(0, roomCount);
			_EnterRoom(roomIndex);
		}
	}
}

void TestClient::_EnterRoom(int roomNum)
{
	if (_isLogin && !_isEnterRoom)
	{
		PacketEnterRoom packetEnterRoom;
		packetEnterRoom.roomNum = roomNum;
		_clientLogic->SendPacket(PacketIndex::ENTER_ROOM, (const char*)&packetEnterRoom);

		_SendMessage();
		_isEnterRoom = true;
		_OutRoom(roomNum);
	}
}

void TestClient::_OutRoom(int roomNum)
{
	if (_isLogin && _isEnterRoom)
	{
		PacketCloseRoom packetCloseRoom;
		packetCloseRoom.roomNum = roomNum;
		_clientLogic->SendPacket(PacketIndex::CLOSE_ROOM, (const char*)&packetCloseRoom);

		_isEnterRoom = false;
	}
}
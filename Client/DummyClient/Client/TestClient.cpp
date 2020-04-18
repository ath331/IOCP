#include "TestClient.h"
#include "ClientLogic.h"
#include "packet.h"

TestClient::TestClient()
{
	_clientLogic = new ClientLogic;
	_clientLogic->Init("127.0.0.1", 9999);
	_clientLogic->Connect();
}

void TestClient::RunTestClient()
{
	//while (true)
	//{
	//	//_Login();
	//	_MakeRoom();
	//}
	_Login();
	_MakeRoom();
}

void TestClient::_Login()
{
	PacketLogin packetLogin;
	memcpy((void*)&packetLogin.id, "a", sizeof("a"));
	memcpy((void*)&packetLogin.pw, "1234", sizeof("1234"));
	_clientLogic->SendPacket(PacketIndex::Login, (const char*)&packetLogin);

	_isLogin = true;
}

void TestClient::_MakeRoom()
{
	if (_isLogin)
	{
		PacketMakeRoom packetMakeRoom;
		memcpy((void*)&packetMakeRoom.roomName, "TestRoom", sizeof("TestRoom"));
		_clientLogic->SendPacket(PacketIndex::MAKE_ROOM, (const char*)&packetMakeRoom);
	}
}

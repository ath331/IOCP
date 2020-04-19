#pragma once

class ClientLogic;
class SafeRandom;
class TestClient
{
public:
	TestClient();
	~TestClient()
	{
		delete _clientLogic;
		delete _safeRandom;
	}

	void RunTestClient();

private:
	SafeRandom* _safeRandom;
	ClientLogic* _clientLogic;

	bool _isLogin = false;
	bool _isEnterRoom = false;

	void _Login();
	void _MakeRoom();
	void _EnterRoom(int roomNum);
	void _OutRoom(int roomNum);
	void _SendMessage();
	void _GetRoomList();
};
#pragma once

class ClientLogic;
class TestClient
{
public:
	TestClient();
	~TestClient()
	{
		delete _clientLogic;
	}

	void RunTestClient();

private:
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
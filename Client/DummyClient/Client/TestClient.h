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

	void _Login();
	void _MakeRoom();
};
#pragma once
#include "ClientLogic.h"
#include <windows.h>
#include <string>

#include "../../Packet/packet.h"
#include "../resource.h"

using namespace std;

enum class DialogType : int
{
	Login,
	Main,
	MakeRoom,
	ChatRoom,
};

class DialogManager
{
public:
	DialogManager(HINSTANCE hInst, ClientLogic* clientLogic)
		: _hInst(hInst), _clientLogic(clientLogic)
	{
		_instance = this;
	}

	void MakeDialog(DialogType dialogType,string roomName = "DEFAULT");

private:
	static DialogManager* _instance;
	ClientLogic* _clientLogic;
	string _roomName;

	HINSTANCE _hInst;
	HBRUSH g_hbrBackground;

	static BOOL CALLBACK DlgProcLogin(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcMain(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcMakeRoom(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static BOOL CALLBACK DlgProcChatRoom(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};
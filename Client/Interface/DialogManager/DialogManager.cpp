#include "DialogManager.h"

#define MWM_SOCKET (WM_USER+1)

DialogManager* DialogManager::_instance = NULL;


void DialogManager::MakeDialog(DialogType dialogType, string roomName)
{
	switch (dialogType)
	{
	case DialogType::Login:
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_ChatLogin), NULL, (DLGPROC)DlgProcLogin);
		break;
	case DialogType::Main:
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_ChatMain), NULL, (DLGPROC)DlgProcMain);
		break;
	case DialogType::MakeRoom:
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_MAKE_ROOM), NULL, (DLGPROC)DlgProcMakeRoom);
		break;
	case DialogType::ChatRoom:
		_instance->_roomName = roomName;
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_ChatROom), NULL, (DLGPROC)DlgProcChatRoom);
		break;
	default:
		break;
	}
}

BOOL CALLBACK DialogManager::DlgProcLogin(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int msgboxID = 0;
	HBRUSH g_hbrBackground = _instance->g_hbrBackground;
	switch (message)
	{
	case WM_INITDIALOG:
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_CTLCOLORDLG:
		return (LONG)g_hbrBackground;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)g_hbrBackground;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_ENTER:
			if (_instance->_clientLogic->Connect() == 0)
			{
				TCHAR name[20];
				GetDlgItemText(hwnd, ID_NAME, name, 10);
				std::string tempName;
				tempName = name;
				_instance->_clientLogic->SetName(tempName);
				PacketLogin packetLogin;
				memcpy((void*)packetLogin.name, name, sizeof(name));
				_instance->_clientLogic->SendPacket<int>(PacketIndex::Login, (const char*)&packetLogin);
				EndDialog(hwnd, 0);
				_instance->MakeDialog(DialogType::Main);
			}
			else
			{
				msgboxID = MessageBox(hwnd, "서버와 연결실패", "연결실패", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}
			break;
		case ID_EXIT:
			msgboxID = MessageBox(hwnd, "프로그램을 종료할까요?", "종료확인", MB_YESNO);

			if (msgboxID == 6)
				EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_DESTROY:
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK DialogManager::DlgProcMain(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int msgboxID = 0;
	HBRUSH g_hbrBackground = _instance->g_hbrBackground;

	switch (message)
	{
	case WM_INITDIALOG:
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_CTLCOLORDLG:
		return (LONG)g_hbrBackground;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)g_hbrBackground;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_MAKE_ROOM:
		{
			_instance->MakeDialog(DialogType::MakeRoom);
		}
		break;
		case IDC_RESET_ROOM:
		{
			HWND listBox = GetDlgItem(hwnd, IDC_LIST_ROOM);
			int listCount = SendMessageA(listBox, LB_GETCOUNT, 0, 0);
			for (int i = 0; i < listCount; i++)
			{
				SendMessageA(listBox, LB_DELETESTRING, i, 0);
			}
			RES_PacketRoomList resPacketRoomList =
				_instance->_clientLogic->SendPacket<RES_PacketRoomList>(PacketIndex::ROOM_LIST, NULL);
			if (resPacketRoomList.maxRoomCount != 0)
			{
				string tempStr = std::to_string(resPacketRoomList.roomNum) + " 번방	" + resPacketRoomList.roomName + "	" + std::to_string(resPacketRoomList.curClientNum) + "/" + std::to_string(resPacketRoomList.maxClientInRoom);
				SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)tempStr.c_str());
			}
		}
		break;

		case ID_ENTER:
		{
			HWND listBox = GetDlgItem(hwnd, IDC_LIST_ROOM);
			int roomIndex = SendMessage(listBox, LB_GETCURSEL, 0, 0);
			if (roomIndex != -1)
			{
				string  roomInfoStr = " "; //초기화안하면 저장이 안되는거 같다
				SendMessage(listBox, LB_GETTEXT, roomIndex, (LPARAM)roomInfoStr.c_str());
				string roomNumStr = roomInfoStr.substr(0, 2);
				PacketEnterRoom packetEnterRoom;
				packetEnterRoom.roomNum = stoi(roomNumStr);
				_instance->_clientLogic->SendPacket<int>(PacketIndex::ENTER_ROOM, (const char*)&packetEnterRoom);
				_instance->MakeDialog(DialogType::ChatRoom, roomInfoStr);
			}
		}
		break;
		case ID_EXIT:
			msgboxID = MessageBox(hwnd, "프로그램을 종료할까요?", "종료확인", MB_YESNO);

			if (msgboxID == 6) //확인버튼 누름
				EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_DESTROY:
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK DialogManager::DlgProcMakeRoom(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HBRUSH g_hbrBackground = _instance->g_hbrBackground;

	switch (message)
	{
	case WM_INITDIALOG:
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
		break;
	case WM_CTLCOLORDLG:
		return (LONG)g_hbrBackground;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)g_hbrBackground;
	}
	break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_ROOM_PRIVATE:
		{
			//TODO : 비밀번호 관련 만들기
			/*if (SendMessage((HWND)IDC_ROOM_PRIVATE, BM_GETCHECK, 0, 0) == BST_UNCHECKED)
			{
				SendMessage((HWND)IDC_ROOM_PRIVATE, BM_SETCHECK, BST_CHECKED, 0);
				SendMessage((HWND)IDC_ROOM_PASSWORD, EM_SETREADONLY, false, 0);
				cout << "BST_CHECKED" << endl;
			}
			else if (SendMessage((HWND)IDC_ROOM_PRIVATE, BM_GETCHECK, 0, 0) == BST_CHECKED)
			{
				SendMessage((HWND)IDC_ROOM_PRIVATE, BM_SETCHECK, BST_UNCHECKED, 0);
				SendMessage((HWND)IDC_ROOM_PASSWORD, EM_SETREADONLY, true, 0);
				cout << "BST_UNCHECKED" << endl;
			}*/
		}
		break;

		case IDOK:
		{
			PacketMakeRoom packetMakeRoom;
			GetDlgItemText(hwnd, IDC_ROOM_NAME, packetMakeRoom.roomName, 10);
			packetMakeRoom.maxClientCount = GetDlgItemInt(hwnd, IDC_ROOM_MAX_CLIENT_NUM, NULL, FALSE);
			if (packetMakeRoom.maxClientCount < 2)
				packetMakeRoom.maxClientCount = 2;
			else if (packetMakeRoom.maxClientCount > 10)
				packetMakeRoom.maxClientCount = 10;

			PacketIndex packetIndex = PacketIndex::MAKE_ROOM;
			int temp = _instance->_clientLogic->SendPacket<int>(packetIndex, (const char*)&packetMakeRoom);
			if (temp != -1 && packetIndex == PacketIndex::MAKE_ROOM)
			{
				//temp = roomNum
				_instance->_roomName = to_string(temp - 1);
				_instance->_roomName += "번방 ";
				_instance->_roomName += packetMakeRoom.roomName;
				EndDialog(hwnd, 0);
				_instance->MakeDialog(DialogType::ChatRoom, _instance->_roomName);
			}
		}
		break;

		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_DESTROY:
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}
	return TRUE;
}
BOOL CALLBACK DialogManager::DlgProcChatRoom(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	WSAAsyncSelect(_instance->_clientLogic->_socket, hwnd, MWM_SOCKET, FD_READ);
	HBRUSH g_hbrBackground = _instance->g_hbrBackground;

	switch (message)
	{
	case WM_INITDIALOG:
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SetWindowText(hwnd, (LPCSTR)_instance->_roomName.c_str());
		break;
	case WM_CLOSE:
		_instance->_clientLogic->SendPacket<int>(PacketIndex::CLOSE_ROOM, NULL);
		EndDialog(hwnd, 0);
		break;
	case WM_CTLCOLORDLG:
		return (LONG)g_hbrBackground;
	case WM_CTLCOLORSTATIC:
	{
		HDC hdcStatic = (HDC)wParam;
		SetTextColor(hdcStatic, RGB(255, 255, 255));
		SetBkMode(hdcStatic, TRANSPARENT);
		return (LONG)g_hbrBackground;
	}
	break;
	case MWM_SOCKET:
	{
		switch (WORD(lParam))
		{
		case FD_READ:
		{
			std::cout << "recv" << std::endl;
		}
		break;

		default:
			break;
		}
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			//TODO : 메시지 송수신을 위헤 IOThread나누기
			PacketSendMessage packetSendMessage;
			string tempStr = " ";
			GetDlgItemText(hwnd, IDC_INPUT_KEY, (LPSTR)tempStr.c_str(), 10);
			memcpy((void*)&packetSendMessage.buffer, tempStr.c_str(), strlen(tempStr.c_str()));
			SetDlgItemText(hwnd, IDC_INPUT_KEY, " ");

			string tempRoomNum = " ";
			GetWindowText(hwnd, (LPSTR)tempRoomNum.c_str(), 2);
			packetSendMessage.roomNum = atoi(tempRoomNum.c_str());
			_instance->_clientLogic->SendPacket<int>(PacketIndex::SEND_MESSAGE, (const char*)&packetSendMessage);
		}
		break;

		
		break;

		case IDCANCEL:
			_instance->_clientLogic->SendPacket<int>(PacketIndex::CLOSE_ROOM, NULL);
			EndDialog(hwnd, 0);
			break;
		}
		break;

	case WM_DESTROY:
		_instance->_clientLogic->SendPacket<int>(PacketIndex::CLOSE_ROOM, NULL);
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
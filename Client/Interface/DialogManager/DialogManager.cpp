#include "DialogManager.h"
#include <iostream>
using namespace std;
#define MWM_SOCKET (WM_USER+1)

DialogManager* DialogManager::_instance = NULL;


void DialogManager::MakeDialog(DialogType dialogType)
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
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_ChatROom), NULL, (DLGPROC)DlgProcChatRoom);
		break;
	case DialogType::MakeID:
		DialogBox(_hInst, MAKEINTRESOURCE(IDD_MAKE_ID), NULL, (DLGPROC)DlgProcMakeID);
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
		{
			if (_instance->_clientLogic->Connect() != 0)
			{
				msgboxID = MessageBox(hwnd, "서버와 연결실패", "연결실패", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}

			char id[20], pw[20];
			GetDlgItemText(hwnd, ID_ID, id, 20);
			GetDlgItemText(hwnd, ID_PW, pw, 20);

			PacketLogin packetLogin;
			memcpy((void*)&packetLogin.id, &id, sizeof(id));
			memcpy((void*)&packetLogin.pw, &pw, sizeof(pw));

			_instance->_clientLogic->SendPacket(sizeof(PacketLogin), (const char*)&packetLogin);
			PacketLogin tempPacketLogin;
			_instance->_clientLogic->RecvPacket(sizeof(PacketLogin));
			memcpy((void*)&tempPacketLogin, &_instance->_clientLogic->buf, sizeof(PacketLogin));

			_instance->_clientLogic->SetName(tempPacketLogin.name);
			if (tempPacketLogin.isSuccessIdCheck)
			{
				EndDialog(hwnd, 0);
				_instance->MakeDialog(DialogType::Main);
			}
			else
			{
				msgboxID = MessageBox(hwnd, "Fail Login", "Login", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}
		}

		break;
		case ID_ENTER2:
		{
			_instance->MakeDialog(DialogType::MakeID);
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
			if (_instance->_clientLogic->GetIsEnteredRoom() == FALSE)
				_instance->MakeDialog(DialogType::MakeRoom);
		}
		break;
		case IDC_CHANGE_NAME:
		{
			if (_instance->_clientLogic->GetIsEnteredRoom() == FALSE)
			{
				char tempName[MAX_NAME_LENGTH];
				GetDlgItemText(hwnd, IDC_NAME_INPUT, tempName, MAX_NAME_LENGTH);
				if (tempName[0] == NULL)
					break;
				string tempNameStr = tempName;
				_instance->_clientLogic->SetName(tempNameStr);

				PacketClientIdInfo packetClientIdInfo;
				memcpy((void*)&packetClientIdInfo.name, tempNameStr.c_str(), sizeof(tempNameStr.c_str()));
				packetClientIdInfo.isChangeName = TRUE;
				_instance->_clientLogic->SendPacket(sizeof(PacketClientIdInfo), (const char*)&packetClientIdInfo);
				msgboxID = MessageBox(hwnd, "닉네임이 변경되었습니다!", "닉네임변경", MB_OK);
				if (msgboxID == 6) //확인버튼 누름
					EndDialog(hwnd, 0);
			}
		}
		break;

		case IDC_RESET_ROOM:
		{
			HWND listBox = GetDlgItem(hwnd, IDC_LIST_ROOM);
			SendMessageA(listBox, LB_RESETCONTENT, 0, 0);

			PacketRoomList packetRoomList;
			_instance->_clientLogic->SendPacket(sizeof(PacketRoomList), (const char*)&packetRoomList);
			RES_PacketRoomList resPacketRoomList;
			_instance->_clientLogic->RecvPacket(sizeof(RES_PacketRoomList));
			memcpy(&resPacketRoomList, &_instance->_clientLogic->buf, sizeof(RES_PacketRoomList));
			if (resPacketRoomList.maxRoomCount != 0)
			{
				for (int i = 0; i < resPacketRoomList.maxRoomCount; i++)
				{
					string tempStr = std::to_string(resPacketRoomList.roomInfoList[i].roomNum) + " 번방	" + resPacketRoomList.roomInfoList[i].roomName + "	" + std::to_string(resPacketRoomList.roomInfoList[i].curClientNum) + "/" + std::to_string(resPacketRoomList.roomInfoList[i].maxClientInRoom);
					SendMessage(listBox, LB_ADDSTRING, 0, (LPARAM)tempStr.c_str());
				}
			}
		}
		break;

		case ID_ENTER:
		{
			if (_instance->_clientLogic->GetIsEnteredRoom() == FALSE)
			{
				HWND listBox = GetDlgItem(hwnd, IDC_LIST_ROOM);
				int roomIndex = SendMessage(listBox, LB_GETCURSEL, 0, 0);
				if (roomIndex != -1)
				{
					string  roomInfoStr = " ";
					SendMessage(listBox, LB_GETTEXT, roomIndex, (LPARAM)roomInfoStr.c_str());
					string roomNumStr = roomInfoStr.substr(0, 2);
					PacketEnterRoom packetEnterRoom;
					packetEnterRoom.roomNum = stoi(roomNumStr);
					_instance->_clientLogic->SendPacket(sizeof(PacketEnterRoom), (const char*)&packetEnterRoom);
					_instance->_clientLogic->SetIsEnteredRoom(TRUE);
					_instance->_roomName = roomInfoStr;
					_instance->MakeDialog(DialogType::ChatRoom);
				}
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
			GetDlgItemText(hwnd, IDC_ROOM_NAME, packetMakeRoom.roomName, 20);
			packetMakeRoom.maxClientCount = GetDlgItemInt(hwnd, IDC_ROOM_MAX_CLIENT_NUM, NULL, FALSE);
			if (packetMakeRoom.maxClientCount < 2)
				packetMakeRoom.maxClientCount = 2;
			else if (packetMakeRoom.maxClientCount > 10)
				packetMakeRoom.maxClientCount = 10;

			_instance->_clientLogic->SendPacket(sizeof(PacketMakeRoom), (const char*)&packetMakeRoom);
			_instance->_clientLogic->RecvPacket(sizeof(RES_PacketMakeRoom));

			RES_PacketMakeRoom resPacketRoomList;
			memcpy(&resPacketRoomList, &_instance->_clientLogic->buf, sizeof(RES_PacketMakeRoom));
			if (resPacketRoomList.roomNum != -1)
			{
				string tempRoomName = to_string(resPacketRoomList.roomNum);
				tempRoomName += "번방 ";
				tempRoomName += packetMakeRoom.roomName;
				_instance->_clientLogic->SetIsEnteredRoom(TRUE);
				_instance->_roomName = tempRoomName;

				PacketEnterRoom packetEnterRoom;
				packetEnterRoom.roomNum = resPacketRoomList.roomNum;
				_instance->_clientLogic->SendPacket(sizeof(PacketEnterRoom), (const char*)&packetEnterRoom);
				_instance->_clientLogic->SetIsEnteredRoom(TRUE);

				EndDialog(hwnd, 0);

				_instance->MakeDialog(DialogType::ChatRoom);
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
	HBRUSH g_hbrBackground = _instance->g_hbrBackground;

	switch (message)
	{
	case WM_INITDIALOG:
	{
		WSAAsyncSelect(_instance->_clientLogic->_socket, hwnd, MWM_SOCKET, FD_READ);
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SetWindowText(hwnd, (LPCSTR)_instance->_roomName.c_str());
	}
	break;
	case WM_CLOSE:
	{
		PacketCloseRoom packetCloseRoom;
		string tempRoomNum = "";
		GetWindowText(hwnd, (char*)tempRoomNum.c_str(), 2);
		packetCloseRoom.roomNum = atoi(tempRoomNum.c_str());
		_instance->_clientLogic->SendPacket(sizeof(PacketCloseRoom), (const char*)&packetCloseRoom);
		EndDialog(hwnd, 0);
		break;
	}
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
			_instance->_clientLogic->RecvPacket(sizeof(PacketSendMessage));
			if (strcmp((const char*)&_instance->_clientLogic->buf[0],"") != 0)
			{
				PacketSendMessage packetSendMessage;
				memcpy((void*)&packetSendMessage.buffer, &_instance->_clientLogic->buf, sizeof(PacketSendMessage));
				HWND listBox = GetDlgItem(hwnd, IDC_LIST2);
				int i = 0;
				i = SendMessage(listBox, LB_GETCURSEL, 0, 0);
				SendMessage(listBox, LB_ADDSTRING, i, (LPARAM)packetSendMessage.buffer);
			}
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
			PacketSendMessage packetSendMessage;
			char tempStr[101];
			GetDlgItemText(hwnd, IDC_INPUT_KEY, (LPSTR)tempStr, 100);
			string tempMessage = "[";
			tempMessage += _instance->_clientLogic->GetName();
			tempMessage += "] : ";
			tempMessage += tempStr;
			memcpy((void*)&packetSendMessage.buffer, tempMessage.c_str(), strlen(tempMessage.c_str()));
			SetDlgItemText(hwnd, IDC_INPUT_KEY, " ");

			string tempRoomNum = " ";
			GetWindowText(hwnd, (LPSTR)tempRoomNum.c_str(), 2);
			packetSendMessage.roomNum = atoi(tempRoomNum.c_str());
			_instance->_clientLogic->SendPacket(sizeof(PacketSendMessage), (const char*)&packetSendMessage);
		}
		break;

		case IDCANCEL:
		{
			PacketCloseRoom packetCloseRoom;
			string tempRoomNum = "";
			GetWindowText(hwnd, (char*)tempRoomNum.c_str(), 2);
			packetCloseRoom.roomNum = atoi(tempRoomNum.c_str());
			_instance->_clientLogic->SendPacket(sizeof(PacketCloseRoom), (const char*)&packetCloseRoom);
			EndDialog(hwnd, 0);
			_instance->_clientLogic->SetIsEnteredRoom(FALSE);
		}
		break;

		}
		break;

	case WM_DESTROY:
		_instance->_clientLogic->SendPacket(sizeof(PacketCloseRoom), NULL);
		DeleteObject(g_hbrBackground);
		break;
	default:
		return FALSE;
	}

	return TRUE;
}
BOOL CALLBACK DialogManager::DlgProcMakeID(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
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
		case IDOK:
		{
			char tempID[20] = "", tempPW[20] = "", tempName[MAX_NAME_LENGTH] = "";
			if (GetDlgItemText(hwnd, ID_INPUT_ID, tempID, sizeof(tempID)) == 0)
				cout << "aa" << endl;
			GetDlgItemText(hwnd, ID_INPUT_PW, tempPW, sizeof(tempPW));
			GetDlgItemText(hwnd, ID_INPUT_NAME, tempName, sizeof(tempName));
			if (tempID[0] == NULL || tempPW[0] == NULL)
			{
				msgboxID = MessageBox(hwnd, "빈칸ㄴㄴ", "MAKE_ID", MB_OK);
				if (msgboxID == 6) //확인버튼 누름
				{
					EndDialog(hwnd, 0);
				}
				break;
			}
			PacketClientIdInfo packetClientIdInfo;
			memcpy((void*)&packetClientIdInfo.id, tempID, sizeof(tempID));
			memcpy((void*)&packetClientIdInfo.pw, tempPW, sizeof(tempPW));
			memcpy((void*)&packetClientIdInfo.name, tempName, sizeof(tempName));

			_instance->_clientLogic->SendPacket(sizeof(PacketClientIdInfo), (const char*)&packetClientIdInfo);

			PacketDBInsertData packetDbInsertData;
			memcpy((void*)&packetDbInsertData, &_instance->_clientLogic->buf, sizeof(PacketDBInsertData));
			if (packetDbInsertData.isSuccessInsertData)
			{
				msgboxID = MessageBox(hwnd, "ID생성", "MAKE_ID", MB_OK);
				if (msgboxID == 6) //확인버튼 누름
				{
					EndDialog(hwnd, 0);
				}
				EndDialog(hwnd, 0);
			}
			else
			{
				msgboxID = MessageBox(hwnd, "ID생성 실패", "MAKE_ID", MB_OK);
				if (msgboxID == 6) //확인버튼 누름
				{
					EndDialog(hwnd, 0);
				}
			}
		}
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
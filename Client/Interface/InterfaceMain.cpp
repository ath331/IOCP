#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "ClientLogic.h"
#include <iostream>
using namespace std;
#include <windows.h>
#include <string>

#include "../../Packet/packet.h"
#include "resource.h"

BOOL CALLBACK DlgProcLogin(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcMain(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcMakeRoom(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DlgProcChatRoom(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);


HBRUSH g_hbrBackground = NULL;
ClientLogic clientLogic;
TCHAR name[20];
BOOL isConnect = false;
HINSTANCE g_hInst;
string _roomName;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	g_hInst = hInstance;
	clientLogic.Init("182.230.110.26", 9999);

	DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ChatLogin), NULL, (DLGPROC)DlgProcLogin);
	if (isConnect)
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_ChatMain), NULL, (DLGPROC)DlgProcMain);

	clientLogic.ExitClient();
	return 0;
}

BOOL CALLBACK DlgProcLogin(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int msgboxID = 0;

	switch (Message)
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
			if (clientLogic.Connect() == 0)
			{
				isConnect = true;
				GetDlgItemText(hwnd, ID_NAME, name, 10);
				std::string tempName;
				tempName = name;
				clientLogic.SetName(tempName);
				PacketLogin* packetLogin = new PacketLogin();
				memcpy((void*)packetLogin->name, name, sizeof(name));
				clientLogic.SendPacket(PacketIndex::Login, (const char*)packetLogin);
				//delete packetLogin;
				EndDialog(hwnd, 0);
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

BOOL CALLBACK DlgProcMain(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	int msgboxID = 0;

	switch (Message)
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
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_MAKE_ROOM), NULL, (DLGPROC)DlgProcMakeRoom);
		}
		break;

		case ID_ENTER:
		{
			//TODO : 방목록체크 데이터 가져와서 접속 패킷전송 구현

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
BOOL CALLBACK DlgProcMakeRoom(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
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
			int temp = clientLogic.SendPacket(packetIndex, (const char*)&packetMakeRoom);
			if (temp != -1 && packetIndex == PacketIndex::MAKE_ROOM)
			{
				//temp = roomNum
				_roomName = to_string(temp);
				_roomName += "번방 ";
				_roomName += packetMakeRoom.roomName;
				EndDialog(hwnd, 0);
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ChatROom), NULL, (DLGPROC)DlgProcChatRoom);
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

BOOL CALLBACK DlgProcChatRoom(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_INITDIALOG:
		g_hbrBackground = CreateSolidBrush(RGB(128, 128, 128));

		SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(NULL,
			MAKEINTRESOURCE(IDI_APPLICATION)));
		SetWindowText(hwnd, _roomName.c_str());
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
		}
		break;

		case IDOK:
		{
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
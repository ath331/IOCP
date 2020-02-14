#include "ClientLogic.h"
#include <windows.h>

#include "resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

HBRUSH g_hbrBackground = NULL;
ClientLogic clientLogic;
BOOL isConnect = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	clientLogic.Init("182.230.110.26", 9999);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ChatLogin), NULL, (DLGPROC)DlgProc);
	if (isConnect)
		DialogBox(hInstance, MAKEINTRESOURCE(IDD_ChatMain), NULL, (DLGPROC)DlgProc);

	clientLogic.ExitClient();
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
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
				EndDialog(hwnd, 0);
			}
			else
			{
				msgboxID = MessageBox(hwnd, L"서버와 연결실패", L"연결실패", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}
			break;
		case ID_EXIT:
			msgboxID = MessageBox(hwnd, L"프로그램을 종료할까요?", L"종료확인", MB_YESNO);

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


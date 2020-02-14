#include "ClientLogic.h"
#include <windows.h>

#include "resource.h"

BOOL CALLBACK DlgProcLogin(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);

HBRUSH g_hbrBackground = NULL;
ClientLogic clientLogic;
BOOL isConnect = false;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	clientLogic.Init("182.230.110.26", 9999);
	DialogBox(hInstance, MAKEINTRESOURCE(IDD_ChatLogin), NULL, (DLGPROC)DlgProcLogin);
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
				EndDialog(hwnd, 0);
			}
			else
			{
				msgboxID = MessageBox(hwnd, L"������ �������", L"�������", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}
			break;
		case ID_EXIT:
			msgboxID = MessageBox(hwnd, L"���α׷��� �����ұ��?", L"����Ȯ��", MB_YESNO);

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
		case ID_ENTER:
			if (clientLogic.Connect() == 0)
			{
				isConnect = true;
				EndDialog(hwnd, 0);
			}
			else
			{
				msgboxID = MessageBox(hwnd, L"������ �������", L"�������", MB_OK);
				if (msgboxID == 6)
					EndDialog(hwnd, 0);
			}
			break;
		case ID_EXIT:
			msgboxID = MessageBox(hwnd, L"���α׷��� �����ұ��?", L"����Ȯ��", MB_YESNO);

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
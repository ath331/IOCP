#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

#include "DialogManager/DialogManager.h"

	HINSTANCE g_hInst;
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	ClientLogic clientLogic;
	clientLogic.Init("20.41.76.224", 3389);

	DialogManager dialogManager(g_hInst, &clientLogic);
	dialogManager.MakeDialog(DialogType::Login);

	clientLogic.ExitClient();
	return 0;
}
#include "s3kse.h"

// GetSystemDefaultLangID()
const char *sClass = "S3KSEWND";
const char *sTitle = "Sonic & Knuckles Collection Save Editor";

HWND hWnd;
HINSTANCE hInst;
LPCSTR lpFile;
bool RegisterWinClass()
{
	if (FindWindow(0, sTitle) || FindWindow(sClass, 0))
		return 1;
	WNDCLASSEX wndClass;
	ZeroMemory(&wndClass,sizeof(WNDCLASSEX));

	memset(&wndClass, 0, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_DBLCLKS;
	wndClass.cbClsExtra = 0;
	wndClass.hInstance = hInst;
	wndClass.lpfnWndProc = (WNDPROC)wndProcMain;
	wndClass.cbWndExtra = 0;
	wndClass.hIcon = LoadIcon(hInst, (LPCSTR)IDI_ICON);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpszClassName = sClass;
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	if (!RegisterClassEx(&wndClass))
	{
		char str[0x40];
		wsprintf(str, "Unable to register class.\n0x%08X", GetLastError());
		MessageBox(0, str, "Error", MB_ICONERROR);
		return 1;
	}
	return 0;
}
bool CreateGameWindow()
{
	hWnd = CreateWindow(sClass, sTitle, WS_CAPTION | DS_MODALFRAME | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, hInst, 0);
	if (hWnd == NULL)
	{
		char str[0x40];
		wsprintf(str, "Unable to create Window.\n0x%08X", GetLastError());
		MessageBox(0, str, "Error", MB_ICONERROR);
		return 1;
	}
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	return 0;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	hInst = hInstance;
	lpFile = lpCmdLine;
	if (RegisterWinClass())
		return -1;
	if (CreateGameWindow())
		return -2;

	MSG msg;
	while(GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}
#include "s3kse.h"

void LoadFile(HWND hWnd)
{
	bool opened = false;
	if (*lpFile != 0)
		opened = save.Init(lpFile);
	if (!opened)
	{
		opened = save.Init(".\\SONIC3K.BIN");
		if (!opened)
			opened = save.Init("C:\\SEGA\\SONIC3K\\SONIC3K.BIN");
		if (!opened)
		{
			char fileName[0x100];
			ZeroMemory(fileName, 0x100);

			OPENFILENAME openFile;
			ZeroMemory(&openFile, sizeof(OPENFILENAME));
			openFile.lStructSize = sizeof(OPENFILENAME);
			openFile.hwndOwner = hWnd;
			openFile.hInstance = hInst;
			openFile.lpstrFilter = "Savegame\0SONIC3K.BIN\0All files\0*.*\0";
			openFile.lpstrFile = fileName;
			openFile.nMaxFile = 0x100;
			openFile.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
			if (GetOpenFileName(&openFile))
				opened = save.Init(openFile.lpstrFile);
		}
	}
	if (!opened) PostQuitMessage(0);
}
void SetWindowSize(HWND hWnd, int width, int height)
{
	RECT rect = {0, 0, width, height};
	AdjustWindowRectEx(&rect, GetWindowLong(hWnd, GWL_STYLE), false, GetWindowLong(hWnd, GWL_EXSTYLE));
	SetWindowPos(hWnd, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW);
}

void CorrectIndexs()
{

	cCharacter = save.GetCharacter();
	cLevel = save.GetLevel();
	cEnding = save.GetEnding();
	if (editingMode == SONIC3K)
	{
		if ((cEnding & 0x7) > 0)
			cLevel += cEnding -1;
	}
}

void DrawSprite(HWND hWnd, HBITMAP hBmp, int x, int y, int width, int height, int orx, int ory, bool flipX, bool flipY)
{
	if (hBmp == NULL) return;
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hBmp);
	if (flipX) StretchBlt(dcBmp, 0, 0, width, height, dcBmp, width-1, 0, -width, height, SRCCOPY);
	if (flipY) StretchBlt(dcBmp, 0, 0, width, height, dcBmp, 0, height-1, width, -height, SRCCOPY);
	TransparentBlt(hDC, x, y, width, height, dcBmp, orx, ory, width, height, RGB(255, 0, 255));
	if (flipX) StretchBlt(dcBmp, 0, 0, width, height, dcBmp, width-1, 0, -width, height, SRCCOPY);
	if (flipY) StretchBlt(dcBmp, 0, 0, width, height, dcBmp, 0, height-1, width, -height, SRCCOPY);
	DeleteDC(dcBmp);
}
void DrawSprite(HWND hWnd, HBITMAP hBmp, int x, int y, int width, int height, int orx, int ory)
{
	if (hBmp == NULL) return;
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hBmp);
	TransparentBlt(hDC, x, y, width, height, dcBmp, orx, ory, width, height, RGB(255, 0, 255));
	DeleteDC(dcBmp);
}
void DrawBitmap(HWND hWnd, HBITMAP hBmp, int x, int y, int width, int height)
{
	if (hBmp == NULL) return;
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hBmp);
	BitBlt(hDC, x, y, width, height, dcBmp, 0, 0, SRCCOPY);
	DeleteDC(dcBmp);
}
void PrintChar(HBITMAP hFont, char character, int x, int y)
{
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hFont);
	TransparentBlt(hDC, x, y, 8, 8, dcBmp, character * 8, 0, 8, 8, RGB(255, 0, 255));
	DeleteDC(dcBmp);
}
void PrintText(HBITMAP hFont, LPCSTR text, int x, int y)
{
	size_t length = strlen(text);
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hFont);
	for(unsigned int i=0; i<length; i++)
	{
		TransparentBlt(hDC, x + (i<<3), y, 8, 8, dcBmp, *text++ * 8, 0, 8, 8, RGB(255, 0, 255));
	}
	DeleteDC(dcBmp);
}
void PrintCounter(HBITMAP hFont, BYTE var, int x, int y)
{
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hFont);
	TransparentBlt(hDC, x + 0, y, 8, 11, dcBmp, var/10 <<3, 0, 8, 11, RGB(255, 0, 255));
	TransparentBlt(hDC, x + 8, y, 8, 11, dcBmp, var%10 <<3, 0, 8, 11, RGB(255, 0, 255));
	DeleteDC(dcBmp);
}
BYTE emeraldsCoordY[] = {_slotY + 154, _slotY + 172};
BYTE emeraldsCoordX[] = {
	_slotX +  4, _slotX + 24, 
	_slotX + 44, _slotX + 64,
	_slotX + 16, _slotX + 36,
	_slotX + 56, 0};
void DrawEmeralds(HWND hWnd)
{
	if (hEmeralds == NULL) return;
	HDC dcBmp = CreateCompatibleDC(hDC);
	SelectObject(dcBmp, hEmeralds);
	
	if (editingMode == SONIC3K)
	{
		BYTE enableSuper = (cEmeralds & 0xAAA8) == 0xAAA8 ? 128 : 0;
		for(int i=0x4000, eindex=0; i!=1; i>>=2, eindex++)
		{
			if (cEmeralds & i)
				TransparentBlt(hDC, emeraldsCoordX[eindex], emeraldsCoordY[eindex>>2], 16, 13, dcBmp, (eindex<<4) + enableSuper, 0, 16, 13, RGB(255, 0, 255));
			else
				TransparentBlt(hDC, emeraldsCoordX[eindex], emeraldsCoordY[eindex>>2], 16, 13, dcBmp, 112 + enableSuper, 0, 16, 13, RGB(255, 0, 255));
		}
	}
	else if (editingMode == SONIC3)
	{
		for(int i=0x80, eindex=0; i!=1; i>>=1, eindex++)
		{
			if (cEmeralds & i)
				TransparentBlt(hDC, emeraldsCoordX[eindex], emeraldsCoordY[eindex>>2], 16, 13, dcBmp, (eindex<<4), 0, 16, 13, RGB(255, 0, 255));
			else
				TransparentBlt(hDC, emeraldsCoordX[eindex], emeraldsCoordY[eindex>>2], 16, 13, dcBmp, 112, 0, 16, 13, RGB(255, 0, 255));
		}
	}

	DeleteDC(dcBmp);
}
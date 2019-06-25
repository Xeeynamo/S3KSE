#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "SaveGameS3K.h"
#include "resource.h"

#pragma comment(lib, "comdlg32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Msimg32.lib") // Used for TransparentBlt

struct Settings
{
	unsigned char vsync, framecount, framecapture, frameskip, debugmode, modex, nose, cdcheck;
	unsigned char bgmmode, gamemode, gametype, language;
};

extern SaveGameS3K save;
extern Settings sets;
extern LPCSTR lpFile;
extern HINSTANCE hInst;
LRESULT CALLBACK wndProcMain(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK wndProcTimeAttack(HWND, UINT, WPARAM, LPARAM);

// Painting vars
extern HDC hDC;
extern HBITMAP hEmeralds;

// General vars
extern BYTE editingMode;
extern BYTE currentSlot;
extern BYTE cEnding;
extern BYTE cCharacter;
extern BYTE cLevel;
extern WORD cEmeralds;

// Coordinates vars
extern int  setArrow;
extern WORD _slotX;
extern WORD _slotY;
extern BYTE emeraldsCoordX[];
extern BYTE emeraldsCoordY[];

// Setting vars
extern LPCSTR settingNames[];
LPCSTR setBGMMode [];
LPCSTR setGameMode[];
LPCSTR setGameType[];
LPCSTR setLanguage[];

// Functions.cpp
void LoadFile(HWND);
void LoadSettingsFile();
void SetSettingsValue(LPCSTR, int);
void SetWindowSize(HWND, int, int);
void CorrectIndexs();
void DrawBitmap(HWND, HBITMAP, int, int, int, int);
void DrawSprite(HWND, HBITMAP, int, int, int, int, int, int, bool, bool);
void DrawSprite(HWND, HBITMAP, int, int, int, int, int, int);
void PrintChar(HBITMAP, char, int, int);
void PrintText(HBITMAP, LPCSTR, int, int);
void PrintCounter(HBITMAP, BYTE, int, int);
void DrawEmeralds(HWND);
void DrawSlotLines();
void ParseButtons(WPARAM);
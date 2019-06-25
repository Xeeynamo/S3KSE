#include "s3kse.h"
#include <stdio.h>
#define SETTINGS 3

const int NativeWindowWidth = 320;
const int NativeWindowHeight = 224;

WORD _slotX = 14 + 100;
WORD _slotY = 16;

const char delPosAnim[4] = {0, 24, 48, 24};
const char delEggAnim[8] = {0, 0, 0, 0, 48, 0, 0, 48};
const COORD posArrowLevel1 = {_slotX - 12, _slotY + 20};
const COORD posArrowLevel2 = {_slotX + 76, _slotY + 20};
const COORD posArrowEnding1 = {_slotX - 12, _slotY + 76};
const COORD posArrowEnding2 = {_slotX + 76, _slotY + 76};
const COORD posArrowLifes1 = {_slotX - 4, _slotY + 110};
const COORD posArrowLifes2 = {_slotX + 68, _slotY + 110};
const COORD posArrowContinues1 = {_slotX - 4, _slotY + 130};
const COORD posArrowContinues2 = {_slotX + 68, _slotY + 130};
const COORD *getArrowPos[] = {&posArrowLevel1, &posArrowLevel2, &posArrowEnding1, &posArrowEnding2, &posArrowLifes1, &posArrowLifes2, &posArrowContinues1, &posArrowContinues2};
const WORD endingTypes[20] = {IDB_UNKNOW, IDB_END10, IDB_END11, IDB_ENDFUL, // Sonic & Tails
							  IDB_UNKNOW, IDB_END10, IDB_END11, IDB_ENDFUL, // Sonic
							  IDB_UNKNOW, IDB_END20, IDB_END21, IDB_ENDFUL, // Tails
							  IDB_UNKNOW, IDB_END30, IDB_END31, IDB_ENDFUL, // Knux
							  IDB_UNKNOW, IDB_END30, IDB_END31, IDB_ENDFUL,}; // Blue Knux
LPCSTR sEditingMode[] = {"SONIC 3K", "SONIC 3", "TIME ATTACK", "SETTINGS"};
const BYTE arrowLimitLeft[] =  {0, 0xFF, 1 ,  0, 0xFF};
      BYTE arrowLimitRight[] = {0, 0xFF, 99, 99, 0xFF};

SaveGameS3K save;
bool eggDelete;
BYTE editingMode;
BYTE currentSlot;
BYTE cLevel;
BYTE cEnding;
BYTE cCharacter;
BYTE currentLifes;
BYTE currentContinues;
WORD cEmeralds;

PAINTSTRUCT ps;
HDC hMainDC;
HDC hDC;
RECT winRect;

HBITMAP backBuffer;
HBITMAP hFont, hFontRed, hFontCounter;
HBITMAP hWall;
HBITMAP hLevel;
HBITMAP hEndGame;
HBITMAP hCharacter;
HBITMAP hLifes;
HBITMAP hContinues;
HBITMAP hEmeralds;
HBITMAP hArrow;
HBITMAP hShadow;
HBITMAP hDelEgg, hDelPos;
HBITMAP hTAIcons, hTALevels;

HBRUSH brushLevelLine;
HPEN redPen, yellowPen;

enum SizeMode
{
	SizeMode_PixelPerfect,
	SizeMode_Double,
	SizeMode_Triple
};

struct Size
{
	int Width;
	int Height;
};

const LPBYTE arrowValueIndex[] = {&cLevel, &cCharacter, &currentLifes, &currentContinues, &cEnding/*DUMMY*/};
int arrowIndex;
COORD *arrowPos1;
COORD *arrowPos2;
int setArrow;
unsigned int contator;
SizeMode sizeMode = SizeMode_Triple;
bool displayArrow;
bool displayHelp;
DWORD threadArrowTick(HWND hWnd)
{
	contator = 0;
	displayArrow = true;
	while(true)
	{
		contator++;
		if (contator >= 32)
		{
			displayArrow = !displayArrow;
			contator -= 32;
		}
		InvalidateRect(hWnd, 0, false);
		Sleep(15);
	}
	return 0;
}

SizeMode GetRotatedSizeMode(SizeMode sizeMode)
{
	switch (sizeMode)
	{
	case SizeMode_PixelPerfect:
		return SizeMode_Double;
	case SizeMode_Double:
		return SizeMode_Triple;
	case SizeMode_Triple:
		return SizeMode_PixelPerfect;
	default:
		return SizeMode_PixelPerfect;
	}
}

Size GetWindowSize(SizeMode sizeMode)
{
	Size size;

	switch (sizeMode)
	{
	case SizeMode_PixelPerfect:
		return { NativeWindowWidth, NativeWindowHeight };
	case SizeMode_Double:
		return { NativeWindowWidth * 2, NativeWindowHeight * 2 };
	case SizeMode_Triple:
		return { NativeWindowWidth * 3, NativeWindowHeight * 3 };
	default:
		return { NativeWindowWidth, NativeWindowHeight };
	}
}

void RefreshEditor(HWND hWnd)
{
	if (editingMode == SETTINGS)
	{
		InvalidateRect(hWnd, 0, false);
		return;
	}
	if (hLevel != NULL) DeleteObject(hLevel);
	if (save.SaveErased())
		hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_NEWFILE));
	else if (editingMode != TIMEATTACK)
	{
		hCharacter = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CHR00 + save.GetCharacter()));
	    cEmeralds = save.GetEmeralds();
		switch (editingMode)
		{
		case SONIC3K:
		{
			save.SetLevel(cLevel > 13 ? 14 : cLevel);
			cEnding = save.GetEnding();
			arrowLimitRight[0] = 16;
			if (cEnding == 0)
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LV00 + cLevel));
			else
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(endingTypes[cEnding + (save.GetCharacter()<<2)]));

			WORD characterIndex = cCharacter > 1 ? cCharacter -1 : 0;
			hLifes = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LIFESONIC + characterIndex));
			hContinues = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CONTSONIC + characterIndex));
			
			currentLifes = save.GetLifes();
			currentContinues = save.GetContinues();
			break;
		}
		case SONIC3:
			save.SetLevel(cLevel);
			arrowLimitRight[0] = 7;
			switch(cLevel)
			{
			case ANGELISLAND:
			case HYDROCITY:
			case MARBLEGARDEN:
			case CARNIVALNIGHT:
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LV00 + cLevel));
				break;
			case FLYINGBATTERYS3:
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LVFB));
				break;
			case ICECAPS3:
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LV04));
				break;
			case LAUNCHBASES3:
				hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_LV05));
				break;
			case CREDITS:
				if ((cEmeralds&0xFE) != 0xFE)
					hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_END10));
				else
					hLevel = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_END11));
				break;
			}
			break;
		case TIMEATTACK:
			break;
		}
	}
	else
	{
		
	}
	InvalidateRect(hWnd, 0, false);
}

LRESULT CALLBACK wndProcMain(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_F1:
			displayHelp = true;
			break;
		case VK_F4:
		{
			Size size = GetWindowSize(sizeMode = GetRotatedSizeMode(sizeMode));
			SetWindowSize(hWnd, size.Width, size.Height);
		}
		case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37: case 0x38:
		case 0x61: case 0x62: case 0x63: case 0x64: case 0x65: case 0x66: case 0x67: case 0x68:
		{
			if (displayHelp || eggDelete || editingMode==4)
				break;
			register char slot = (wParam-1) & 0xF;
			if (editingMode == SONIC3)
			{
				if (slot > 5) return 0;
			}
			else if (editingMode == TIMEATTACK)
			{
				if (slot > 2) return 0;
			}
			save.SelectSlot(currentSlot = slot);
			CorrectIndexs();
			break;
		}
		case VK_PRIOR:
			if (displayHelp || eggDelete)
				break;
			switch (editingMode)
			{
			case SONIC3K:
				editingMode++;
				break;
			case SONIC3:
				emeraldsCoordY[0] = _slotY + 154;
				emeraldsCoordY[1] = _slotY + 172;
				break;
			case TIMEATTACK:
				DeleteObject(hTAIcons);
				DeleteObject(hTALevels);
				break;
			}
			arrowIndex = 0;
			save.SelectSaveGame((SaveGameType)--editingMode);
			save.SelectSlot(currentSlot = 0);
			CorrectIndexs();
			break;
		case VK_NEXT:
			if (displayHelp || eggDelete)
				break;
			switch (editingMode)
			{
			case SONIC3K:
				emeraldsCoordY[0] = _slotY + 114;
				emeraldsCoordY[1] = _slotY + 132;
				break;
			case SONIC3:
				hTAIcons = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TAICONS));
				hTALevels = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TANAMES));
				break;
			case TIMEATTACK:
				LoadSettingsFile();
				break;
			case SETTINGS:
				editingMode--;
				break;
			}
			arrowIndex = 0;
			save.SelectSaveGame((SaveGameType)++editingMode);
			save.SelectSlot(currentSlot = 0);
			CorrectIndexs();
			break;
		case VK_DELETE:
			if (displayHelp)
				break;
			if (editingMode >= TIMEATTACK)
				break;
			if (save.SaveErased())
				break;
			eggDelete = !eggDelete;
			displayArrow = !displayArrow;
			if (eggDelete)
			{
				hDelEgg = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DELEGG));
				hDelPos= LoadBitmap(hInst, MAKEINTRESOURCE(IDB_DELPOS));
			}
			else
			{
				DeleteObject(hDelEgg);
				DeleteObject(hDelPos);
			}
			break;
		case VK_ESCAPE:
			if (displayHelp)
			{
				displayHelp = false;
				break;
			}
			PostQuitMessage(0);
		case VK_RETURN:
			if (editingMode == SETTINGS)
				break;
			if (displayHelp)
			{
				displayHelp = false;
				break;
			}
			if (save.SaveErased())
			{
				save.InitializeSlot();
				cLevel = 0;
			}
			else if (eggDelete)
			{
				save.EraseSave();
				eggDelete = false;
				displayArrow = !displayArrow;
				DeleteObject(hDelEgg);
				DeleteObject(hDelPos);
			}
			displayHelp = false;
			break;
		}
		
		if (displayHelp || eggDelete)
			break;
		if (editingMode == SETTINGS)
			ParseButtons(wParam);
		else if (!save.SaveErased())
		{
			switch(wParam)
			{
			case VK_LEFT:
				switch(arrowIndex)
				{
				case 0: // Level editor
					if (cLevel != ANGELISLAND)
					{
						if(cLevel > THEDOOMSDAY) save.SetEnding(save.GetEnding()-1);
						cLevel--;
					}
					break;
				case 1: // Character editor
					if (cCharacter > 0) cCharacter--;
					else cCharacter = 4;
					save.SetCharacter(cCharacter);
					break;
				case 2: // Lifes
					if (editingMode == SONIC3)
					{
						(BYTE)cEmeralds--;
						save.SetEmeralds(cEmeralds);
					}
					else if (currentLifes > 1)
					{
						currentLifes--;
						save.SetLifes(currentLifes);
					}
					break;
				case 3: // Continues
					if (currentContinues > 0)
					{
						currentContinues--;
						save.SetContinues(currentContinues);
					}
					break;
				case 4: // Emeralds
					save.SetEmeralds(cEmeralds);
					break;
				}
				break;
			case VK_RIGHT:
				switch(arrowIndex)
				{
				case 0: // Level editor
					if (editingMode == SONIC3K)
					{
						if(cLevel >= THEDOOMSDAY)
						{
							if (cLevel > 15) break;
							save.SetEnding(save.GetEnding()+1);
						}
						cLevel++;
					}
					else if(cLevel != CREDITS) cLevel++;
					break;
				case 1: // Character editor
					if (cCharacter < 4) cCharacter++;
					else cCharacter = 0;
					save.SetCharacter(cCharacter);
					break;
				case 2: // Lifes
					if (editingMode == SONIC3)
					{
						(BYTE)cEmeralds++;
						save.SetEmeralds(cEmeralds);
					}
					else if (currentLifes < 99)
					{
						currentLifes++;
						save.SetLifes(currentLifes);
					}
					break;
				case 3: // Continues
					if (currentContinues < 99)
					{
						currentContinues++;
						save.SetContinues(currentContinues);
					}
					break;
				case 4: // Emeralds
					save.SetEmeralds(cEmeralds);
					break;
				}
				break;
			case VK_SPACE:
				if (editingMode == SONIC3K)
				{
					if ((cEmeralds & 0x5554) == 0x5554)
						cEmeralds ^= 0x5554;
					else
						cEmeralds |= 0x5554;
				}
				if (editingMode == SONIC3)
				{
					if ((cEmeralds & 0xFE) == 0xFE)
						cEmeralds ^= 0xFE;
					else
						cEmeralds |= 0xFE;
				}
				save.SetEmeralds(cEmeralds);
				break;
			case VK_SHIFT:
				if (editingMode == SONIC3K)
				{
					if ((cEmeralds & 0xAAA8) == 0xAAA8)
						cEmeralds ^= 0xAAA8;
					else
						cEmeralds |= 0xAAA8;
					save.SetEmeralds(cEmeralds);
				}
				break;
			case VK_UP:
				if (editingMode != TIMEATTACK)
				{
					if (arrowIndex>0) arrowIndex--;
					arrowPos1 = (COORD*)getArrowPos[arrowIndex<<1];
					arrowPos2 = (COORD*)getArrowPos[(arrowIndex<<1) + 1];
				}
				break;
			case VK_DOWN:
				if (editingMode != TIMEATTACK)
				{
					if (editingMode == SONIC3K)
					{
						if (arrowIndex<4)
						{
							arrowIndex++;
							arrowPos1 = (COORD*)getArrowPos[arrowIndex<<1];
							arrowPos2 = (COORD*)getArrowPos[(arrowIndex<<1) + 1];
						}
					}
					else if (arrowIndex<1)
					{
						arrowIndex++;
						arrowPos1 = (COORD*)getArrowPos[arrowIndex<<1];
						arrowPos2 = (COORD*)getArrowPos[(arrowIndex<<1) + 1];
					}
					else if (arrowIndex==1)
					{
						arrowIndex++;
						arrowPos1 = (COORD*)getArrowPos[8];
						arrowPos2 = (COORD*)getArrowPos[9];
					}
				}
				break;
			}
		}
		RefreshEditor(hWnd);
		break;
	case WM_PAINT:
		hMainDC = BeginPaint(hWnd, &ps);
		hDC = CreateCompatibleDC(hMainDC);
		backBuffer = CreateCompatibleBitmap(hMainDC, ps.rcPaint.right, ps.rcPaint.bottom);
		SelectObject(hDC, backBuffer);

		DrawBitmap(hWnd, hWall, 0, 0, 320, 224);

		if (!displayHelp) // If the Helping mode is disable display all stuff
		{
			if (editingMode < TIMEATTACK)
			{
				DrawSprite(hWnd, hShadow, _slotX, _slotY, 46, 97, 0, 0, false, false);
				DrawSprite(hWnd, hShadow, _slotX+45, _slotY, 46, 97, 0, 0, true, false);
				DrawSprite(hWnd, hShadow, _slotX, _slotY+96, 46, 97, 0, 0, false, true);
				DrawSprite(hWnd, hShadow, _slotX+45, _slotY+96, 46, 97, 0, 0, true, true);
				DrawSlotLines();
				DrawBitmap(hWnd, hLevel, _slotX, _slotY, 80, 56);

				if (!save.SaveErased()) // If the savegame isn't an erased file then display all stuff
				{
					DrawSprite(hWnd, hCharacter, _slotX + 14, _slotY + 68, 52, 40, 0, 0);
					DrawEmeralds(hWnd);

					if (editingMode == SONIC3K) // Display Lifes, Continues and relative counters
					{
						DrawSprite(hWnd, hLifes      , _slotX + 20, _slotY + 110, 16, 16, 0 , 0);
						DrawSprite(hWnd, hContinues  , _slotX + 20, _slotY + 126, 16, 24, 0 , 0);
						DrawSprite(hWnd, hFontCounter, _slotX + 36, _slotY + 112, 8 , 11, 80, 0);
						DrawSprite(hWnd, hFontCounter, _slotX + 36, _slotY + 134, 8 , 11, 80, 0);
						PrintCounter(hFontCounter, currentLifes, _slotX + 44, _slotY + 112);
						PrintCounter(hFontCounter, currentContinues, _slotX + 44, _slotY + 134);
					}
				
					if (editingMode == SONIC3K && cLevel > 13) // Display correct CLEAR in Sonic 3K
						PrintText(hFontRed, "CLEAR", _slotX + 20, _slotY+58);
					else if (editingMode == SONIC3 && cLevel == 7) // Display correct CLEAR in Sonic 3
						PrintText(hFontRed, "CLEAR", _slotX + 20, _slotY+58);
					else // Display the number of Zone
					{
						int tmp = cLevel + 1;
						if (editingMode == SONIC3 && tmp > 5) tmp -= 1; // Fix strange indexing on Sonic 3
						PrintText(hFontRed, "ZONE", _slotX + 14, _slotY+58);
						PrintChar(hFontRed, tmp / 10 + 0x30, _slotX + 52, _slotY+58); // Display decimal unit
						PrintChar(hFontRed, tmp % 10 + 0x30, _slotX + 60, _slotY+58); // Display first unit
					}
				
					if (eggDelete) // If EggDelete mode is enable display eggman sprite and relative poster
					{
						DrawSprite(hWnd, hDelPos, _slotX+16, _slotY-4, 48, 24, 0, delPosAnim[(contator>>3)]);
						DrawSprite(hWnd, hDelEgg, _slotX+16, _slotY-4 + 24, 48, 48, 0, delEggAnim[(contator>>2)]);
					}
					else if (displayArrow) // If EggDelete mode is disable then display the arrows
					{
						if (*arrowValueIndex[arrowIndex] != arrowLimitLeft[arrowIndex]) // If the value isn't min display left arrow
							DrawSprite(hWnd, hArrow, arrowPos1->X, arrowPos1->Y, 16, 16, 0, 0);
						if (*arrowValueIndex[arrowIndex] != arrowLimitRight[arrowIndex]) // If the value isn't max display right arrow
								DrawSprite(hWnd, hArrow, arrowPos2->X, arrowPos2->Y, 16, 16, 0, 0, true, false);
					}
				}

				/*char debug[0x10];
				sprintf(debug, "EMERALDS: %04X", save.GetEmeralds());
				PrintText(hFontRed, debug, 200, 16);
				sprintf(debug, "ENDING: %02X", save.GetEnding());
				PrintText(hFontRed, debug, 200, 24);
				sprintf(debug, "LEVEL: %02X", save.GetLevel());
				PrintText(hFontRed, debug, 200, 32);*/
			}
			else if (editingMode == TIMEATTACK)
			{
				PrintText(hFontRed, "NOT IMPLEMENTED YET", 80, 32);
				PrintText(hFontRed, "PLEASE BE PATIENT", 88, 40);
								
				DrawSprite(hWnd, hTAIcons, 72*0 + 52, 80, 64, 48, 0, 48*0);
				DrawSprite(hWnd, hTAIcons, 72*1 + 52, 80, 64, 48, 0, 48*1);
				DrawSprite(hWnd, hTAIcons, 72*2 + 52, 80, 64, 48, 0, 48*2);
				
				DrawSprite(hWnd, hTAIcons, 72*0 + 88,136, 64, 48, 0, 48*3);
				DrawSprite(hWnd, hTAIcons, 72*1 + 88,136, 64, 48, 0, 48*4);

			}
			else if (editingMode == SETTINGS)
			{
				PrintText(hFont, ">", 8, 24 + (setArrow*8));
				PrintText(hFont, "[ ] VERTICAL SYNC"   , 24, 24);
				PrintText(hFont, "[ ] SHOW FRAME COUNT", 24, 32);
				PrintText(hFont, "[ ] FRAME CAPTURE"   , 24, 40);
				PrintText(hFont, "[ ] FRAME SKIP"      , 24, 48);
				PrintText(hFont, "[ ] DEBUG MODE"      , 24, 56);
				PrintText(hFont, "[ ] MODE X"          , 24, 64);
				PrintText(hFont, "[ ] NO SOUND EFFECTS", 24, 72);
				PrintText(hFont, "[ ] CD CHECK"        , 24, 80);
				PrintText(hFont, "BGM MODE:"           , 24, 88);
				PrintText(hFont, "GAME MODE:"          , 24, 96);
				PrintText(hFont, "GAME TYPE:"          , 24,104);
				PrintText(hFont, "LANGUAGE:"           , 24,112);

				unsigned char *psets = (unsigned char*)&sets;
				for(int i=0; i<8; i++)
				{
					if (*psets++)
						PrintText(hFont, "X", 32, 24 + (i<<3));
				}
				PrintText(hFont, setBGMMode [*psets++], 112, 88);
				PrintText(hFont, setGameMode[*psets++], 112, 96);
				PrintText(hFont, setGameType[*psets++], 112,104);
				PrintText(hFont, setLanguage[*psets++], 112,112);
			}

			PrintText(hFont, "PRESS F1 FOR HELP", 0, 208);
			PrintText(hFont, "MODE:", 0, 0);
			PrintText(hFont, sEditingMode[editingMode], 48, 0);
			if (editingMode != SETTINGS)
			{
				PrintText(hFont, "SLOT:", 152, 0);
				PrintChar(hFont, currentSlot + 0x31, 200, 0);
			}
		}
		else // Display Helping mode texts
		{
			PrintText(hFont, "WELCOME TO SONIC 3K SAVE EDITOR", 32, 8);
			PrintText(hFont, "--------------------------------------", 8, 16);
			PrintText(hFont, "F1        DISPLAY THIS GUIDE"       , 16,  32);
			PrintText(hFont, "F4        INCREASE SIZE OF EDITOR"  , 16,  40);
			PrintText(hFont, "1-8 NUMS  CHANGE SLOT"              , 16,  48);
			PrintText(hFont, "PAGE U/D  SWITCH EDITING MODE"      , 16,  56);
			PrintText(hFont, "L/R       CHANGE VALUE"             , 16,  64);
			PrintText(hFont, "U/D       SELECT VALUE TO CHANGE"   , 16,  72);
			PrintText(hFont, "DELETE    SELECT DELETE MODE"       , 16,  80);
			PrintText(hFont, "SPACE     ENABLE ALL EMERALDS"      , 16,  88);
			PrintText(hFont, "SHIFT     ENABLE HIDDEN PALACE'S"   , 16,  96);
			PrintText(hFont, "          S.STAGES IN SONIC 3K MODE", 16, 104);
			PrintText(hFont, "RETURN    CREATE NEW SAVE OR"       , 16, 112);
			PrintText(hFont, "          DELETE IT ON DELETE MODE" , 16, 120);
			PrintText(hFont, "ESCAPE    LEAVE FROM THIS GUIDE OR" , 16, 128);
			PrintText(hFont, "          GET OUT FROM THE EDITOR"  , 16, 136);
			
			PrintText(hFont, "VERSION 0.6 BETA", 16, 160);
			PrintText(hFont, "COMPILED ON 25/SEPTEMBER/2010", 16, 168);
			PrintText(hFont, "DEVELOPED BY XEEYNAMO", 16, 176);
			PrintText(hFont, "HTTP://DIGITALWORK.ALTERVISTA.ORG", 16, 184);
		}
		PrintText(hFont, "S3KSE 0.6 25/09/10 DEVELOPED BY XEEYNAMO", 0, 216);
		
		if (sizeMode != SizeMode_PixelPerfect)
		{
			Size size = GetWindowSize(sizeMode);
			StretchBlt(hDC, 0, 0, size.Width, size.Height, hDC, 0, 0, NativeWindowWidth, NativeWindowHeight, SRCCOPY);
			BitBlt(hMainDC, 0, 0, size.Width, size.Height, hDC, 0, 0, SRCCOPY);
		}
		else
		{
			BitBlt(hMainDC, 0, 0, NativeWindowWidth, NativeWindowHeight, hDC, 0, 0, SRCCOPY);
		}

		DeleteObject(backBuffer);
		DeleteDC(hDC);
		EndPaint(hWnd, &ps);
		break;
	case WM_CREATE:	
	{
		LoadFile(hWnd);
		save.SelectSaveGame(SONIC3K);
		save.SelectSlot(currentSlot);
		CorrectIndexs();
		RefreshEditor(hWnd);
		
		arrowPos1 = (COORD*)getArrowPos[arrowIndex<<1];
		arrowPos2 = (COORD*)getArrowPos[(arrowIndex<<1) + 1];
		hWall = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_WALL));
		hArrow = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ARROW));
		hEmeralds = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EMERALDS));
		hFont = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONT));
		hFontRed = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONTRED));
		hFontCounter = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FONTCOUNTER));
		hShadow = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SLOTSHADOW));

		brushLevelLine = CreateSolidBrush(RGB(34, 34, 34));
		redPen = CreatePen(PS_SOLID, 1, RGB(136, 34, 0));
		yellowPen = CreatePen(PS_SOLID, 2, RGB(238, 238, 0));

		Size size = GetWindowSize(sizeMode);
		SetWindowSize(hWnd, size.Width, size.Height);
		DWORD thID;
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)threadArrowTick, hWnd, 0, &thID);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void DrawSlotLines()
{
	RECT rect = {_slotX-2, _slotY-2, _slotX+82, _slotY+58};
	FillRect(hDC, &rect, brushLevelLine);

	POINT pRed[5] = {_slotX-3, _slotY+186, _slotX-3, _slotY-3, _slotX+82, _slotY-3, _slotX+82, _slotY+186, _slotX-3, _slotY+186};
	SelectObject(hDC, redPen);
	Polyline(hDC, pRed, 5);

	POINT pYellow[5] = {_slotX-4, _slotY+188, _slotX-4, _slotY-4, _slotX+84, _slotY-4, _slotX+84, _slotY+188, _slotX-4, _slotY+188};
	SelectObject(hDC, yellowPen);
	Polyline(hDC, pYellow, 5);
}
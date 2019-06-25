#include "s3kse.h"
#define GetSettingsValue(key, defaultval) GetPrivateProfileInt("Debug", key, defaultval, "C:\\WINDOWS\\S3K.INI")

LPCSTR settingNames[] = {"EnableFrameDrop", "ShowFrameCount", "EnableFrameCapture", "MukaiDropFrame", "DebugMode",
	"UseModeX", "NoSoundEffect", "CDCheckEnable", "BGMMode", "SonicGameMode", "GameMode", "LangMode"};
const unsigned char settingDefault[] = {0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0};
LPCSTR setBGMMode [] = {"NO MUSIC", "FM SYNTHESIZER", "GENERAL MIDI"};
LPCSTR setGameMode[] = {"SONIC 3 AND KNUCKLES", "SONIC AND KNUCKLES", "SONIC 3"};
LPCSTR setGameType[] = {"STANDARD", "DEMO", "TRIAL"};
LPCSTR setLanguage[] = {"ENGLISH", "FRENCH", "GERMAN", "SPANISH"};

Settings sets;
void LoadSettingsFile()
{
	OFSTRUCT fileStruct;
	if (OpenFile("C:\\WINDOWS\\S3K.INI", &fileStruct, OF_EXIST) == HFILE_ERROR)
	{
		HFILE hFile;
		if (hFile = OpenFile("C:\\WINDOWS\\S3K.INI", &fileStruct, OF_CREATE) == HFILE_ERROR)
		{
			MessageBox(NULL, "Unable to load or create S3K.ini file", "Error", MB_ICONERROR);
			return;
		}
		DWORD bytewritten;
		WriteFile((HANDLE)hFile, "[Debug]\n", 9, &bytewritten, NULL);
		CloseHandle((HANDLE)hFile);
	}
	unsigned char *pSets = (unsigned char*)&sets;
	for(int i=0; i<12; i++)
	{
		*pSets++ = GetSettingsValue(settingNames[i], settingDefault[i]);
	}
}

void WritePrivateProfileInt(LPCSTR lpszSection, LPCSTR lpszEntry, int value, LPCSTR lpszFilename)
{
	char szConverted[8];
	_itoa (value, szConverted, 10);
	WritePrivateProfileString(lpszSection, lpszEntry, szConverted, lpszFilename);
}
void SetSettingsValue(LPCSTR lpszEntry, int value)
{
	WritePrivateProfileInt("Debug", lpszEntry, value, "C:\\WINDOWS\\S3K.INI");
}
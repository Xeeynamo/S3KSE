#include "s3kse.h"
#include <stdio.h>
#define SETTINGS 3

void ParseButtons(WPARAM wParam)
{
	switch(editingMode)
	{
	case SONIC3K:
		switch(wParam)
		{
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_SPACE:
			break;
		}
		break;
	case SONIC3:
		switch(wParam)
		{
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_SPACE:
			break;
		}
		break;
	case TIMEATTACK:
		switch(wParam)
		{
		case VK_LEFT:
			break;
		case VK_RIGHT:
			break;
		case VK_UP:
			break;
		case VK_DOWN:
			break;
		case VK_SPACE:
			break;
		}
		break;
	case SETTINGS:
		unsigned char *pset = (unsigned char*)&sets + setArrow;
		switch(wParam)
		{
		case VK_BACK:
		case VK_LEFT:
			if (setArrow < 8)
			{
				*pset = (bool)!*pset;
			}
			else if (setArrow < 11)
			{
				*pset = (*pset - 1) % 3;
				if (*pset == 0xFF)
					*pset = 2;
			}
			else
			{
				*pset = *pset - 1 % 4;
				if (*pset == 0xFF)
					*pset = 3;
			}
			SetSettingsValue(settingNames[setArrow], *pset);
			break;
		case VK_RIGHT:
		case VK_SPACE:
		case VK_RETURN:
			if (setArrow < 8)
			{
				*pset = (bool)!*pset;
			}
			else if (setArrow < 11)
			{
				*pset = (*pset + 1) % 3;
			}
			else
			{
				*pset = *pset + 1 & 3;
			}
			SetSettingsValue(settingNames[setArrow], *pset);
			break;
		case VK_UP:
			if (setArrow > 0) setArrow--;
			break;
		case VK_DOWN:
			if (setArrow < 11) setArrow++;
			break;
		}
		break;
	}
}
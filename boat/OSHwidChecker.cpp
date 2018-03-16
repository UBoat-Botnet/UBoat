#include <windows.h>
#include <string>
#include <sstream>
#include "OSHwidChecker.h"

bool getHwid(char* str, int bufferSize)
{
	HW_PROFILE_INFOA hpi;
	ZeroMemory(&hpi, sizeof(HW_PROFILE_INFOA));
	GetCurrentHwProfileA(&hpi);
	strcpy(str,  hpi.szHwProfileGuid);
	return true;
}
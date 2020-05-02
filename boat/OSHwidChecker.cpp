#include "WindowsCompat.h"
#include <string>
#include <sstream>
#include "OSHwidChecker.h"

bool getHwid(char* str, int bufferSize)
{
#ifdef __WIN32
	HW_PROFILE_INFOA hpi;
	ZeroMemory(&hpi, sizeof(HW_PROFILE_INFOA));
	GetCurrentHwProfileA(&hpi);
	strcpy(str,  hpi.szHwProfileGuid);
#else
	strncpy(str, "9440bc71-2126-4f88-9960-5fccd865afc2", bufferSize);
#endif
	return true;
}

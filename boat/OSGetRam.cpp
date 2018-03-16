#include <windows.h>
#include <string>
#include <sstream>
#include "OSGetRam.h"

bool getram(char* str, int bufferLength)
{
	unsigned long long rams;
	GetPhysicallyInstalledSystemMemory(&rams);
	_itoa(rams, str, 10);
	return true;
}
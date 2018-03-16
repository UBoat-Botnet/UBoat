#include <windows.h>
#include <string>
#include <sstream>
#include "OSCpuChecker.h"

bool getCpuName(char* str, int buffferLenght)
{
	// Get extended ids.
	int CPUInfo[4] = { -1 };
	__cpuid(CPUInfo, 0x80000000);
	unsigned int nExIds = CPUInfo[0];

	// Get the information associated with each extended ID.
	char CPUBrandString[0x40] = { 0 };
	for (unsigned int i = 0x80000000; i <= nExIds; ++i)
	{
		__cpuid(CPUInfo, i);

		// Interpret CPU brand string and cache information.
		if (i == 0x80000002)
		{
			memcpy(CPUBrandString,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000003)
		{
			memcpy(CPUBrandString + 16,
				CPUInfo,
				sizeof(CPUInfo));
		}
		else if (i == 0x80000004)
		{
			memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
		
		/*SYSTEM_INFO si;
		ZeroMemory(&si, sizeof(SYSTEM_INFO));
		GetSystemInfo(&si);
		__asm {int 3}*/
	}
	strcpy(str, CPUBrandString);
	return true;
}
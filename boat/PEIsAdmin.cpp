#include <windows.h>
#include <string>
#include <sstream>
#include "PEIsAdmin.h"

bool check(char* str, int buffferLenght)
{
	bool fRet = false;
	HANDLE hToken = NULL;
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize)) {
			fRet = Elevation.TokenIsElevated;
		}
	}
	if (hToken) {
		CloseHandle(hToken);
	}

	if(fRet)
		strcpy(str, "true");
	else
		strcpy(str, "false");

	return fRet;
}
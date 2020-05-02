#include "WindowsCompat.h"
#include <string>
#include <sstream>
#include "PEIsAdmin.h"
#if ! defined(__WIN32)
#include <unistd.h>
#endif

bool check(char* str, int buffferLenght)
{
	bool fRet = false;
#if defined(__WIN32)
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
#else
	uid_t uid = getuid();
	fRet = uid == 0;
#endif
	if(fRet)
		strcpy(str, "true");
	else
		strcpy(str, "false");

	return fRet;
}

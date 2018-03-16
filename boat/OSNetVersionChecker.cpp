#include <windows.h>
#include <string>
#include <sstream>
#include "OSNetVersionChecker.h"
#pragma comment (lib, "Advapi32.lib");

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383

bool QueryKey(HKEY hKey, char* netVer)
{
	TCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
	DWORD    cbName;                   // size of name string 
	TCHAR    achClass[MAX_PATH] = TEXT("");  // buffer for class name 
	DWORD    cchClassName = MAX_PATH;  // size of class string 
	DWORD    cSubKeys = 0;               // number of subkeys 
	DWORD    cbMaxSubKey;              // longest subkey size 
	DWORD    cchMaxClass;              // longest class string 
	DWORD    cValues;              // number of values for key 
	DWORD    cchMaxValue;          // longest value name 
	DWORD    cbMaxValueData;       // longest value data 
	DWORD    cbSecurityDescriptor; // size of security descriptor 
	FILETIME ftLastWriteTime;      // last write time 

	DWORD i, retCode;

	TCHAR  achValue[MAX_VALUE_NAME];
	DWORD cchValue = MAX_VALUE_NAME;

	// Get the class name and the value count. 
	retCode = RegQueryInfoKey(
		hKey,                    // key handle 
		achClass,                // buffer for class name 
		&cchClassName,           // size of class string 
		NULL,                    // reserved 
		&cSubKeys,               // number of subkeys 
		&cbMaxSubKey,            // longest subkey size 
		&cchMaxClass,            // longest class string 
		&cValues,                // number of values for this key 
		&cchMaxValue,            // longest value name 
		&cbMaxValueData,         // longest value data 
		&cbSecurityDescriptor,   // security descriptor 
		&ftLastWriteTime);       // last write time 

								 // Enumerate the subkeys, until RegEnumKeyEx fails.

	if (cSubKeys)
	{
		cbName = MAX_KEY_LENGTH;
		retCode = RegEnumKeyEx(hKey, cSubKeys - 1,
			achKey,
			&cbName,
			NULL,
			NULL,
			NULL,
			&ftLastWriteTime);
		if (retCode == ERROR_SUCCESS)
		{
			std::wstring wArchKey = achKey;
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wArchKey[0], (int)wArchKey.size(), NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_UTF8, 0, &wArchKey[0], (int)wArchKey.size(), &strTo[0], size_needed, NULL, NULL);

			strcpy(netVer, strTo.c_str());

			return true;
		}

	}
	return false;
}
bool getLastestNETinstalled(char* str, int bufferLenght)
{
	HKEY hTestKey;
	char* netVer = (char*)malloc(bufferLenght);

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\NET Framework Setup\\NDP"),
		0,
		KEY_READ,
		&hTestKey) == ERROR_SUCCESS
		)
	{
		QueryKey(hTestKey, netVer);
		strcpy(str, netVer);
	}

	RegCloseKey(hTestKey);

	free(netVer);

	return true;

}
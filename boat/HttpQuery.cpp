#include "HttpQuery.h"
#include "StringOperations.h"
#include <windows.h>
#include <stdio.h>
#include <string>
#include "Sockets.h"

#define POST_BUFFER_LEN (1024 * 1024 * (7)) //5MB buffer you made it 15MB? you did that oh lol 40 for now.

char to_hex(char code) {
	static char hex[] = "0123456789abcdef";
	return hex[code & 15];
}

char *url_encode(const char *str) {
	char *pstr = (char*)str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
	while (*pstr) {
		if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~')
			*pbuf++ = *pstr;
		else if (*pstr == ' ')
			*pbuf++ = '+';
		else
			*pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
		pstr++;
	}
	*pbuf = '\0';
	return buf;
}

static char* response_code_name = "HTTP Response Code";

int HTTPQuery(char* host, char* path, NameValuePair** headers, NameValuePair*** responseHeaders, char** responseData, int* dataLength, bool isString, char* protocol, NameValuePair** postData)
{
	*responseData = 0;
	*responseHeaders = 0;

	sockaddr_in input;
	if(!GetSocketAddress(host, &input)) return -1;
	SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (s == INVALID_SOCKET) return -1;

	int cRes = connect(s, (sockaddr*)&input, sizeof(input));
	if (cRes == SOCKET_ERROR) return -1;

	char* sendBuffer = (char*)malloc(512);

	char* postBuffer = (char*)GlobalAlloc(GMEM_FIXED, POST_BUFFER_LEN);
	ZeroMemory(postBuffer, POST_BUFFER_LEN);


	sprintf_s(sendBuffer, 512, "%s %s HTTP/1.0\r\nHost: %s\r\n", protocol, path, host);

	send(s, sendBuffer, strlen(sendBuffer), 0);

	if (headers != NULL)
	{
		int i = 0;
		while (headers[i] != NULL)
		{
			sprintf_s(sendBuffer, 512, "%s: %s\r\n", headers[i]->Name, headers[i]->Value);
			send(s, sendBuffer, strlen(sendBuffer), 0);
			i++;
		}
	}

	if (postData != NULL)
	{
		char* contentType = "Content-Type: application/x-www-form-urlencoded\r\n";
		send(s, contentType, strlen(contentType), 0);
		int i = 0;
		bool posted = false;
		while (postData[i] != NULL)
		{
			char* encoded = url_encode(postData[i]->Value);
			int len = strlen(encoded);
			sprintf_s(postBuffer + strlen(postBuffer), POST_BUFFER_LEN - strlen(postBuffer), "%s%s=%s", posted ? "&" : "", postData[i]->Name, encoded);
			free(encoded);
			i++;
			posted = true;
		}
		sprintf_s(sendBuffer, 512, "Content-Length: %d\r\n", strlen(postBuffer));
		send(s, sendBuffer, strlen(sendBuffer), 0);
	}

	send(s, "\r\n", 2, 0);

	if (strlen(postBuffer) != 0)
	{
		send(s, postBuffer, strlen(postBuffer), 0);
	}

	GlobalFree((HGLOBAL)postBuffer);
	free(sendBuffer);

	char* recvBuffer = (char*)malloc(2048);
	char* originalBuffer = recvBuffer;
	
	do
	{
		int r = recv(s, recvBuffer++, 1, 0);
		if (r == 0)
		{
			free(recvBuffer);
			return -1;
		}
	} while ((recvBuffer - originalBuffer < 4) || *(recvBuffer - 1) != '\n' || *(recvBuffer - 2) != '\r' || *(recvBuffer - 3) != '\n' || *(recvBuffer - 4) != '\r');

	int r = recvBuffer - originalBuffer;

	char* headerBuffer = (char*)malloc(r - 4 + 1);
	headerBuffer[r - 4] = 0;
	memcpy_s(headerBuffer, r - 4, originalBuffer, r - 4);
	free(originalBuffer);

	int httpResponseValue = 0;
	int headerCount;
	char** splitResults = SplitString(headerBuffer, "\r\n", &headerCount, true);
	NameValuePair** returnHeaders = (NameValuePair**)malloc(sizeof(NameValuePair*) * (headerCount + 1));

	for (int i = 0; i < headerCount; i++)
	{
		int num;
		char** splitResult2 = SplitString(splitResults[i], ": ", &num, false);
		
		if (num == 2)
		{
			int nameLength = strlen(splitResult2[0]);
			int valueLength = strlen(splitResult2[1]);
			char* name = (char*)malloc(nameLength + 1);
			name[nameLength] = 0;
			char* value = (char*)malloc(valueLength + 1);
			value[valueLength] = 0;

			memcpy_s(name, nameLength, splitResult2[0], nameLength);
			memcpy_s(value, valueLength, splitResult2[1], valueLength);

			returnHeaders[i] = new NameValuePair(name, value);
		}
		else if (num == 1)
		{
			int valueLength = strlen(splitResult2[0]);
			char* value = (char*)malloc(valueLength + 1);
			value[valueLength] = 0;
			memcpy_s(value, valueLength, splitResult2[0], valueLength);

			returnHeaders[i] = new NameValuePair(i == 0 ? response_code_name : 0, value);
			
			int numResultCode;
			char** splitResult3 = SplitString(value, " ", &numResultCode, false);

			if (numResultCode == 3)
			{
				DWORD* http = (DWORD*)"HTTP";
				if(*(DWORD*)splitResult3[0] == *http)
					httpResponseValue = std::stoi(splitResult3[1]);
			}

			FreeSplitStringBuffer(splitResult3, numResultCode);
		}			

		FreeSplitStringBuffer(splitResult2, num);
	}

	returnHeaders[headerCount] = 0;
	*responseHeaders = returnHeaders;
	free(headerBuffer);

	FreeSplitStringBuffer(splitResults, headerCount);

	int contentLength = 0;

	for (int i = 0; i < headerCount; i++)
	{
		if (returnHeaders[i]->Name != 0 && strcmp(returnHeaders[i]->Name, "Content-Length") == 0)
			contentLength = std::stoi(returnHeaders[i]->Value);
	}
	
	if (contentLength == 0) contentLength = 1024 * 1024 * 2; //2MB buffer.

	recvBuffer = (char*)malloc(contentLength + (isString ? 1: 0));
	originalBuffer = recvBuffer;
	if(isString) originalBuffer[contentLength] = 0;

	int readLength = 0;

	while (readLength != contentLength)
	{
		int read = recv(s, recvBuffer, contentLength - readLength, 0);

		if (read > 0)
		{
			readLength += read;
			recvBuffer += read;
		}
		else if (read == 0 && readLength != contentLength)
		{
			char* newBuffer = (char*)malloc(readLength + 1);
			newBuffer[readLength] = 0;
			memcpy_s(newBuffer, readLength, originalBuffer, readLength);
			free(originalBuffer);
			originalBuffer = newBuffer;
			break;
		}
		else
		{
			int error = WSAGetLastError();
			int b = 0;
		}
	}

	*dataLength = readLength;
	*responseData = originalBuffer;

	return httpResponseValue;
}

void FreeHTTPResponse(NameValuePair** header, char* data)
{
	if(data) free(data);
	NameValuePair** headerOrig = header;
	while (header && *header)
	{
		if((*header)->Name && (*header)->Name != response_code_name) free((void*)(*header)->Name);
		free((void*)(*header)->Value);
		free(*header);
		header++;
	}
	free(headerOrig);
}
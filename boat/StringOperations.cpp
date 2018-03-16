#include "StringOperations.h"

char** SplitString(char* string, char* delimiter, int* numElements, bool ignoreEmpty)
{
	int nDelimiterLen = strlen(delimiter);
	int nStringLen = strlen(string);
	int nDelimMatch = 0;
	*numElements = 0;

	if (nDelimiterLen == 0 || nDelimiterLen >= nStringLen)
		return (char**)NULL;

	char* initial = string;
	char** buffer = (char**)malloc(((nStringLen / nDelimiterLen) + 1) * sizeof(char*));
	ZeroMemory(buffer, ((nStringLen / nDelimiterLen) + 1) * sizeof(char*));

	for (int i = 0; i < nStringLen; i++)
	{
		if (string[i] == delimiter[nDelimMatch])
			nDelimMatch++;
		else {
			nDelimMatch = 0;
			if (string[i] == delimiter[nDelimMatch])
				nDelimMatch++;
		}

		if (nDelimMatch == nDelimiterLen)
		{
			nDelimMatch = 0;

			char* data = initial;
			int dataLength = string + i - initial + 1 - nDelimiterLen;
			initial = string + i + 1;

			if (dataLength == 0 && ignoreEmpty)
				continue;

			buffer[*numElements] = (char*)malloc(dataLength + 1);
			if(dataLength) memcpy_s(buffer[*numElements], dataLength, data, dataLength);
			buffer[*numElements][dataLength] = '\0';
			*numElements = *numElements + 1;
		}
		
		if (i == nStringLen - 1)
		{
			char* data = initial;
			int dataLength = string + i - initial + 1;

			if (dataLength == 0 && ignoreEmpty)
				continue;
				
			buffer[*numElements] = (char*)malloc(dataLength + 1);
			if (dataLength) memcpy_s(buffer[*numElements], dataLength, data, dataLength);
			buffer[*numElements][dataLength] = '\0';
			*numElements = *numElements + 1;
		}
	}

	return buffer;
}

void FreeSplitStringBuffer(char** buffer, int numElements)
{
	for (int i = 0; i < numElements; i++) 
	{
		free(buffer[i]);
	}

	free(buffer);
}

char* JoinString(char** strings, int numElements, int* outputLength) {
	int size = 0;
	for (int i = 0; i < numElements; i++)
		size += strlen(strings[i]);

	char* finalBuffer = (char*)malloc(size + 1);
	finalBuffer[size] = 0;

	for (int i = 0, index = 0; i < numElements; i++) {
		memcpy_s(finalBuffer + index, size - index, strings[i], strlen(strings[i]));
		index += strlen(strings[i]);
	}

	*outputLength = size;
	return finalBuffer;
}

void FreeJoinStringBuffer(char* buffer) {
	free(buffer);
}
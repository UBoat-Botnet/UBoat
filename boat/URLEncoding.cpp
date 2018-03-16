#include "URLEncoding.h"
#include  <Windows.h>

static char* hexTable = "0123456789ABCDEF";

char* URLEncode(char* input, int length)
{
	int nInputLen = length;
	int nOutputLen = nInputLen * 3;

	char* output = (char*)malloc(nOutputLen + 1);
	char* retOutput = output;
	output[nOutputLen] = '\0';

	for (int i = 0; i < nInputLen; i++)
	{
		*output++ = '%';
		*output++ = hexTable[(input[i] & 0xF0) >> 4];
		*output++ = hexTable[(input[i] & 0x0F)];
	}

	return retOutput;
}

char* URLDecode(char* input, int* outputLength)
{
	int nInputLen = strlen(input);
	if ((nInputLen % 3) != 0)
		return NULL;
	int nOutputLen = nInputLen / 3;
	*outputLength = nOutputLen;
	char* output = (char*)malloc(nOutputLen + 1);
	output[nOutputLen] = '\0';

	char* workingInput = input;
	char* retOutput = output;

	for (int i = 0; i < nOutputLen; i++)
	{
		workingInput++; // dispose of '%'
		char charValue = 0;
		char working = *workingInput++;
		char actual;
		for (actual = 0; actual < 16; actual++)
		{
			if (hexTable[actual] == working)
				break;
		}
		charValue += (actual << 4);
		working = *workingInput++;
		for (actual = 0; actual < 16; actual++)
		{
			if (hexTable[actual] == working)
				break;
		}
		charValue += actual;
		*output++ = charValue;
	}

	return retOutput;
}

void FreeURLEncodeResult(char* input)
{
	free(input);
}

void FreeURLDecodeResult(char* input)
{
	free(input);
}
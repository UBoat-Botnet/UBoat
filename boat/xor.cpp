#include "xor.h"
#include <Windows.h>

char* XORInputKey(char* input, char* key, int inputLength, int keyLength) 
{
	char* output = (char*)malloc(inputLength + 1);
	output[inputLength] = 0;
	for (int i = 0; i < inputLength; i++)
		output[i] = input[i] ^ key[i % keyLength];
	return output;
}

void FreeXORResult(char* input) 
{
	free(input);
}

char* GenerateKey(int length)
{
	char* buffer = (char*)malloc(length);
	for (int i = 0; i < length; i++)
		buffer[i] = rand() * 255;
	return buffer;
}

void FreeKey(char* key)
{
	free(key);
}
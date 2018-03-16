#include "WebSafeEncryption.h"
#include "URLEncoding.h"
#include <Windows.h>

char* GetEncodedXorResult(char* input, char* key, int inputLength, int keyLength, char** outputKey)
{
	char* xoredInput = XORInputKey(input, key, inputLength, keyLength);
	char* encoded = URLEncode(xoredInput, inputLength);
	*outputKey = URLEncode(key, keyLength);
	FreeXORResult(xoredInput);
	return encoded;
}

char* GetDecodedXorResult(char* input, char* key, int* outputLength)
{
	int inputLength, keyLength;
	char* decodedKey = URLDecode(key, &keyLength);
	char* decodedInput = URLDecode(input, &inputLength);

	char* output = XORInputKey(decodedInput, decodedKey, inputLength, keyLength);
	FreeURLDecodeResult(decodedInput);
	FreeURLDecodeResult(decodedKey);

	*outputLength = inputLength;

	return output;
}

void FreeEncodedXorResult(char* input, char* key)
{
	FreeURLEncodeResult(input);
	FreeURLEncodeResult(key);
}

void FreeDecodedXorResult(char* input)
{
	FreeXORResult(input);
}
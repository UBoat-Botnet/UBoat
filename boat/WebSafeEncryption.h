#ifndef WEBSAFEENCRYPTION_H
#define WEBSAFEENCRYPTION_H

#include "xor.h"

char* GetEncodedXorResult(char* input, char* key, int inputLength, int keyLength, char** outputKey);
char* GetDecodedXorResult(char* input, char* key, int* outputLength);
void FreeEncodedXorResult(char* input, char* key);
void FreeDecodedXorResult(char* input);


#endif
#ifndef URLENCODING_H
#define URLENCODING_H

char* URLEncode(char* input, int length);
char* URLDecode(char* input, int* outputLength);

void FreeURLEncodeResult(char* input);
void FreeURLDecodeResult(char* input);

#endif
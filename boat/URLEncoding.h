#ifndef URLENCODING_H
#define URLENCODING_H

char* URLEncode(const char* input, int length);
char* URLDecode(const char* input, int* outputLength);

void FreeURLEncodeResult(char* input);
void FreeURLDecodeResult(char* input);

#endif

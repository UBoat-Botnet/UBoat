#ifndef XOR_H
#define XOR_H

char* XORInputKey(char* input, char* key, int inputLength, int keyLength);
void FreeXORResult(char* input);
char* GenerateKey(int length);
void FreeKey(char* key);

#endif
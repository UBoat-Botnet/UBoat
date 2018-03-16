#ifndef GATEHELPER_H
#define GATEHELPER_H

void SetBotId(char* newId);
char* GetBotId();
char* SendCommandWithDecodedResponse(char* command);
void FreeDecodedResponse(char* response);

#endif
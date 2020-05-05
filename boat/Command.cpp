/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <cstring>
#include <sstream>
#include <string>

#include "Commands.h"
#include "StringOperations.h"
#include "URLEncoding.h"

std::string CreateCommand(int id, int type, std::string data)
{
    std::stringstream output;
    std::string encoded = URLEncode(data);

    output << id << '|' << type << '|' << encoded;

    return output.str();
}

Command ParseCommand(const std::string& command)
{
    Command ret;
    std::vector<std::string> splitResult = SplitString(command, '|');

    if (splitResult.size() != 3)
        return ret;

    ret.id = std::stoi(splitResult[0], 0, 10);
    ret.type = std::stoi(splitResult[1], 0, 10);
    ret.data = URLDecode(splitResult[2]);

    return ret;
}

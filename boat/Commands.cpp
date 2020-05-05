/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <cstring>
#include <sstream>
#include <string>

#include "Commands.hpp"
#include "StringOperations.h"
#include "URLEncoding.h"

namespace UBoat {
namespace Commands {

    std::string Create(int id, int type, std::string data)
    {
        std::stringstream output;
        std::string encoded = URLEncode(data);

        output << id << '|' << type << '|' << encoded;

        return output.str();
    }

    Command Parse(const std::string& command)
    {
        Command ret;
        std::vector<std::string> parts = SplitString(command, '|');

        if (parts.size() != 3)
            return ret;

        ret.id = std::stoi(parts[0], 0, 10);
        ret.type = std::stoi(parts[1], 0, 10);
        ret.data = URLDecode(parts[2]);

        return ret;
    }
} // Command
} // UBoat

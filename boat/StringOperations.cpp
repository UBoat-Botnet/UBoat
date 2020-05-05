/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <string>
#include <sstream>
#include <vector>

#include "StringOperations.h"

std::vector<std::string> SplitString(const std::string &s, char delim)
{
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(std::move(item));
    }

    return elems;
}

/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <string>

#include "xor.h"

std::string XORInputKey(const std::string& input, const std::string& key)
{
    std::string output;

    for (std::string::size_type i = 0; i < input.size(); i++)
        output += input[i] ^ key[i % key.size()];

    return output;
}

std::string GenerateKey(int lenght)
{
    std::string key;

    for (std::string::size_type i = 0; i < lenght; i++)
        key += rand() * 255;

    return key;
}

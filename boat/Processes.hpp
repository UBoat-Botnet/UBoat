/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#pragma once

#include <string>

namespace UBoat {
namespace Processes {
    std::string GetResult(const std::string& processName, const std::string& args);
    bool Start(const std::string& processName, const std::string& args);
} // Processes
} // UBoat

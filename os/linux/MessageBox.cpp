/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <iostream>
#include <string>

namespace UBoat {
namespace MessageBox {
    void Display(std::string message, std::string title = "")
    {
        std::cout << "[+] MessageBox(`" << message << "`, `" << title << "`)" << std::endl;
    }
} // MessageBox
} // UBoat

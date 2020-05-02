#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stddef.h>

#include "WindowsCompat.h"
// #include <errors.h>

char* _itoa(int value, char* buffer, int radix)
{
    sprintf(buffer, "%d", value);
    return buffer;
}

errno_t _itoa_s(int value, char* buffer, size_t size, int radix)
{
    return snprintf(buffer, size, "%d", value);
}

errno_t memcpy_s(void* dest, rsize_t destsz, const void* src, rsize_t count)
{
    memcpy(dest, src, count);
    return 0;
}

DWORD GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer)
{
    strncpy(lpBuffer, "/temp", nBufferLength);
    return strlen(lpBuffer);
}

HANDLE CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile)
{
#ifdef _DEBUG_
    printf("[+] Creando archivo (%s).\n", lpFileName);
#endif
    FILE* fp = fopen(lpFileName, "w");
    if (fp == NULL) {
        perror("CreateFileA");
        exit(EXIT_FAILURE);
    }
    return fp;
}

BOOL WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped)
{
#ifdef _DEBUG_
    printf("[+] WriteFile (%lu bytes).\n", nNumberOfBytesToWrite);
#endif
    size_t size = fwrite(lpBuffer, (size_t)nNumberOfBytesToWrite, 1, (FILE *)hFile);
#ifdef _DEBUG_
    printf("[+] %ld bytes written.\n", size);
#endif

    if (size > 0)
        return 1;

    return 0;
}

BOOL CloseHandle(HANDLE hObject)
{
#ifdef _DEBUG_
    printf("[+] CloseHandle.\n");
#endif
    fclose((FILE *)hObject);
    return 1;
}

BOOL GetPhysicallyInstalledSystemMemory(PULONGLONG TotalMemoryInKilobytes)
{
#if defined(__linux__)
    struct sysinfo info;
    sysinfo(&info);
    *TotalMemoryInKilobytes = info.totalram;
#elif defined(__APPLE__)
    char* p;
    size_t len;
    sysctlbyname("hw.memsize", NULL, &len, NULL, 0);
    p = (char *)malloc(len);
    sysctlbyname("hw.memsize", p, &len, NULL, 0);
    *TotalMemoryInKilobytes = atoi(p);
#endif
    return 1;
}

int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType)
{
#ifdef _DEBUG_
    printf("[+] MessageBoxA(%d, '%s', '%s', %d).\n", hWnd, lpText, lpCaption, uType);
#endif
    return 0;
}

HANDLE CreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    __drv_aliasesMem LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId)
{
#ifdef _DEBUG_
    printf("[+] CreateThread().\n");
#endif
    // pthread_t threadId;
    //
    // int err = pthread_create(&threadId, NULL, lpStartAddress, (void *)lpParameter);
    // if (err) {
    //     fprintf(stderr, "[+] Thread creation failed: %s.\n", strerror(err));
    //     return NULL;
    // }
    //
    // printf("[+ ] Thread created with ID: %d.\n", threadId);
    // *lpThreadId = threadId;

    // return threadId;
    return NULL;
}

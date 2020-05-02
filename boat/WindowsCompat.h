#ifndef __WINDOWSCOMPAT_H__
#define __WINDOWSCOMPAT_H__
#if defined(_WIN32)
#include <windows.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <stdio.h>
#include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>
// #ifndef __STDC_WANT_LIB_EXT1__
// #define __STDC_WANT_LIB_EXT1__ 1
// #endif // #ifndef __STDC_WANT_LIB_EXT1__
#include <stdlib.h>
#define __STDC_WANT_LIB_EXT1__ 1
#include <pthread.h>
#include <string.h>
#include <stddef.h>
#if defined(__linux__)
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#endif // --

#define _SAL_VERSION_CHECK(_A)
#define __nullterminated _SAL_VERSION_CHECK(__nullterminated)
#define __drv_aliasesMem

/**
 * ==================
 *     WinDef.h
 * ==================
 */
#define CONST const
#define VOID void
#define CALLBACK __stdcall

#define far /* nothing */
#define near /* nothing */

typedef int BOOL;
typedef char CHAR;
typedef CHAR* LPSTR;
typedef __nullterminated CONST CHAR* LPCSTR;

typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef DWORD* LPDWORD;

typedef void* LPVOID;
typedef CONST void* LPCVOID;
typedef void* PVOID;
typedef PVOID HANDLE;

typedef unsigned char BYTE;
typedef BYTE far* LPBYTE;

typedef __nullterminated CONST CHAR* LPCSTR;

typedef HANDLE HWND;
typedef HANDLE HHOOK;
typedef HANDLE HGLOBAL;

typedef long LONG_PTR;
typedef LONG_PTR LRESULT;

typedef unsigned long ULONG_PTR;
typedef unsigned long long ULONGLONG;
typedef ULONGLONG* PULONGLONG;
typedef ULONG_PTR SIZE_T;

typedef wchar_t WCHAR;
typedef WCHAR* LPWSTR;

#ifdef __linux__
typedef int errno_t;
typedef size_t rsize_t;
#define __stdcall
#endif

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

#define RtlZeroMemory(Destination, Length) memset((Destination), 0, (Length))
#define ZeroMemory RtlZeroMemory

char* _itoa(int value, char* buffer, int radix);
errno_t _itoa_s(int value, char* buffer, size_t size, int radix);
errno_t memcpy_s(void* dest, rsize_t destsz, const void* src, rsize_t count);

DWORD GetTempPathA(DWORD nBufferLength, LPSTR lpBuffer);

/**
 * ==================
 *     fileapi.h
 * ==================
 */
#define GENERIC_READ 0x80000000
#define GENERIC_WRITE 0x40000000
#define CREATE_ALWAYS 2

typedef void* LPOVERLAPPED;

HANDLE CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile);
BOOL WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped);
BOOL CloseHandle(HANDLE hObject);

/**
 * ==================
 *     processthreadsapi.h
 * ==================
 */
typedef DWORD(__stdcall* LPTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);

typedef struct _STARTUPINFOA {
    DWORD cb;
    LPSTR lpReserved;
    LPSTR lpDesktop;
    LPSTR lpTitle;
    DWORD dwX;
    DWORD dwY;
    DWORD dwXSize;
    DWORD dwYSize;
    DWORD dwXCountChars;
    DWORD dwYCountChars;
    DWORD dwFillAttribute;
    DWORD dwFlags;
    WORD wShowWindow;
    WORD cbReserved2;
    LPBYTE lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

HANDLE CreateThread(
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    __drv_aliasesMem LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId);

#define ExitProcess(uExitCode) exit(uExitCode)

/**
  * ==================
  *     sysinfoapi.h
  * ==================
  */
BOOL GetPhysicallyInstalledSystemMemory(PULONGLONG TotalMemoryInKilobytes);

#define sprintf_s(dest, len, format, ...) snprintf(dest, len, format, __VA_ARGS__)
#define GMEM_FIXED 0x0000
#define GlobalAlloc(uFlags, dwBytes) malloc(dwBytes)
#define GlobalFree(ptr) \
    if (ptr)            \
    free(ptr)

#define WSAGetLastError() 1

int MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType);

#endif
#endif // #ifndef __WINDOWSCOMPAT_H__

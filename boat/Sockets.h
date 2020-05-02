#ifndef SOCKETS_H
#define SOCKETS_H

#if defined(__WIN32)
#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>
#elif defined(__linux__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
typedef int SOCKET;
/*  these constants are useful, but appear to be specific to */
/*  winsock; so, I define them here and use them as if they */
/*  were standard */
#define INVALID_SOCKET ((SOCKET)(~0))
#define SOCKET_ERROR (-1)
#endif // defined(__linux__) || define(__APPLE__)

bool GetSocketAddress(char* host, sockaddr_in* address);

#endif

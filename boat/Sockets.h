#ifndef SOCKETS_H
#define SOCKETS_H

#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>
#include <winsock2.h>

bool GetSocketAddress(char* host, sockaddr_in* address);

#endif
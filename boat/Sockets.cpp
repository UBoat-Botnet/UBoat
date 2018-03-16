#include "Sockets.h"

bool GetSocketAddress(char* host, sockaddr_in* address)
{
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(host, "http", &hints, &result)) return false;

	*address = *(sockaddr_in*)(result[0].ai_addr);
	freeaddrinfo(result);
	return true;
}
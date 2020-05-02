#include "Sockets.h"
#include "WindowsCompat.h"

bool GetSocketAddress(char* host, sockaddr_in* address)
{
#ifdef _DEBUG_
	printf("[?] GetSocketAddress(`%s`, sockaddr_in* address).\n", host);
#endif
	struct addrinfo* result = NULL;
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	if (getaddrinfo(host, NULL, &hints, &result)) return false;

#ifdef _DEBUG_
	char addrstr[100];
	inet_ntop(result->ai_family, &((struct sockaddr_in *)result->ai_addr)
				   ->sin_addr, addrstr, 100);
	printf("[?] GetSocketAddress -> %s\n", addrstr);
#endif

	// *address = *(sockaddr_in*)(result[0].ai_addr);
	*address = *(struct sockaddr_in *)result->ai_addr;
	freeaddrinfo(result);
	return true;
}

#include "Sockets.h"
#include "SocketInit.h"

bool SocketStartup()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) return false;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		WSACleanup();
		return false;
	}
	else
		return true;
}

void SocketCleanup()
{
	WSACleanup();
}
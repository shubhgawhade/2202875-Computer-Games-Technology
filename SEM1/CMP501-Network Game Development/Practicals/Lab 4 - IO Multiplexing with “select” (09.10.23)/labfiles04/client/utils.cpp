/* Utility functions for the client and server */

#include <WinSock2.h>
#include <cstdio>
#include <cstdlib>

#include "utils.h"

#pragma comment(lib, "ws2_32.lib")


void die(const char *message) {
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}

void startWinSock() {
	// We want version 2.2.
	WSADATA w;
	int error = WSAStartup(0x0202, &w);
	if (error != 0)
	{
		die("WSAStartup failed");
	}
	if (w.wVersion != 0x0202)
	{
		die("Wrong WinSock version");
	}
}
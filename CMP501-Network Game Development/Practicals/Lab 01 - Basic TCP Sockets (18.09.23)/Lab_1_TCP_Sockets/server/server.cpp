/*	CMP303 TCP server example - by Henry Fortuna and Adam Sampson
	Updated by Gaz Robinson & Andrei Boiko

	A simple TCP server that waits for a connection.
	When a connection is made, the server sends "hello" to the client.
	It then repeats back anything it receives from the client.
	All the calls are blocking -- so this program only handles
	one connection at a time.
*/

#include <cctype>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

// The IP address for the server to listen on
#define SERVERIP "127.0.0.1"

// The TCP port number for the server to listen on
#define SERVERPORT 5555

// The message the server will send when the client connects
#define WELCOME "hello"

// The (fixed) size of message that we send between the two programs
#define MESSAGESIZE 40


// Prototypes
void die(const char *message);


int main()
{
	printf("Echo Server\n");

	// Initialise the WinSock library -- we want version 2.2.
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

	// Create a TCP socket that we'll use to listen for connections.
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	// FIXME: we should test for error here
	if(serverSocket == INVALID_SOCKET)
	{
		die("Failed Creation of Socket");
	}

	// Fill out a sockaddr_in structure to describe the address we'll listen on.
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(SERVERIP);
	// htons converts the port number to network byte order (big-endian).
	serverAddr.sin_port = htons(SERVERPORT);

	// Bind the server socket to that address.
	if (bind(serverSocket, (const sockaddr *) &serverAddr, sizeof(serverAddr)) != 0)
	{
		die("bind failed");
	}

	// ntohs does the opposite of htons.
	printf("Server socket bound to address %s, port %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

	// Make the socket listen for connections.
	if (listen(serverSocket, 1) != 0)
	{
		die("listen failed");
	}

	printf("Server socket listening\n");

	while (true)
	{
		printf("Waiting for a connection...\n");

		// Accept a new connection to the server socket.
		// This gives us back a new socket connected to the client, and
		// also fills in an address structure with the client's address.
		sockaddr_in clientAddr;
		int addrSize = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &addrSize);
		if (clientSocket == INVALID_SOCKET)
		{
			continue;
			// die("accept failed");
			// FIXME: in a real server, we wouldn't want the server to crash if
			// it failed to accept a connection -- recover more effectively!
		}

		printf("Client has connected from IP address %s, port %d!\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

		// We'll use this array to hold the messages we exchange with the client.
		char buffer[MESSAGESIZE];

		// Fill the buffer with - characters to start with.
		memset(buffer, '-', MESSAGESIZE);

		// Send a welcome message to the client.
		memcpy(buffer, WELCOME, strlen(WELCOME));
		// FIXME: check for errors from send
		if(send(clientSocket, buffer, MESSAGESIZE, 0) == SOCKET_ERROR)
		{
			die("Send message error");
		}

		while (true)
		{
			// Receive as much data from the client as will fit in the buffer.
			int count = recv(clientSocket, buffer, MESSAGESIZE, 0);
			// FIXME: check for errors from recv
			// if(count == SOCKET_ERROR)
			// {
			// 	die("recv error");
			// }
			if (count <= 0) {
				printf("Client closed connection\n");
				break;
			}
			if (count != MESSAGESIZE) {
				die("Got strange-sized message from client");
			}
			if (memcmp(buffer, "quit", 4) == 0) {
				printf("Client asked to quit\n");
				break;
			}

			// (Note that recv will not write a \0 at the end of the message it's
			// received -- so we can't just use it as a C-style string directly
			// without writing the \0 ourself.)
			for (int i=0; i<=sizeof(buffer);i++)
			{
				buffer[i] = std::toupper(buffer[i]);
			}
			printf("Received %d bytes from the client: '", count);
			fwrite(buffer, 1, count, stdout);
			printf("'\n");

			// Send the same data back to the client.
			// FIXME: check for errors from send
			if(send(clientSocket, buffer, MESSAGESIZE, 0) == SOCKET_ERROR)
			{
				die("Send error");
			}
		}

		printf("Closing connection\n");

		// Close the connection.
		closesocket(clientSocket);
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}


// Print an error message and exit.
void die(const char *message) {
	fprintf(stderr, "Error: %s (WSAGetLastError() = %d)\n", message, WSAGetLastError());

#ifdef _DEBUG
	// Debug build -- drop the program into the debugger.
	abort();
#else
	exit(1);
#endif
}
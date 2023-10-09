/*	AG0907 select-based TCP server example - by Henry Fortuna and Adam Sampson
	Adapted for CMP303/501 by Andrei Boiko and Gaz Robinson

	A simple server that waits for connections.
	The server repeats back anything it receives from the client.
*/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <list>
#include <WinSock2.h>

#include "connection.h"
#include "utils.h"

#pragma comment(lib, "ws2_32.lib")


// The IP address for the server to listen on
#define SERVERIP "127.0.0.1"

// The TCP port number for the server to listen on
#define SERVERPORT 5555


int main()
{
	printf("Echo Server\n");

	startWinSock();

	// Create a TCP socket that we'll use to listen for connections.
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET)
	{
		die("socket failed");
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

	// The list of clients currently connected to the server.
	std::list<Connection *> conns;

	// The server's main loop, where we'll wait for new connections to
	// come in, or for new data to appear on our existing connections.
	while (true)
	{
		// The structure that describes the set of sockets we're interested in.
		fd_set readable;
		FD_ZERO(&readable);

		fd_set writeable;
		FD_ZERO(&writeable);

		// Add the server socket, which will become "readable" if there's a new
		// connection to accept.
		FD_SET(serverSocket, &readable, &writeable);

		// Add all of the connected clients' sockets.
		for (auto conn: conns)
		{
			if (conn->wantRead())
			{
				FD_SET(conn->sock(), &readable);
			}
		}

		// The structure that describes how long to wait for something to happen.
		timeval timeout;
		// We want a 2.5-second timeout.
		timeout.tv_sec = 2;
		timeout.tv_usec = 500000;

		// Wait for one of the sockets to become readable.
		// (We can only get away with passing 0 for the first argument here because
		// we're on Windows -- other sockets implementations need a proper value there.)
		int count = select(0, &readable, &writeable, NULL, &timeout);
		if (count == SOCKET_ERROR)
		{
			die("select failed");
		}
		printf("%d clients; %d sockets are ready\n", conns.size(), count);
		// readable now tells us which sockets are ready.
		// If count == 0 (i.e. no sockets are ready) then the timeout occurred.

		// Is there a new connection to accept?
		if (FD_ISSET(serverSocket, &readable))
		{
			// Accept a new connection to the server socket.
			// This gives us back a new socket connected to the client, and
			// also fills in an address structure with the client's address.
			sockaddr_in clientAddr;
			int addrSize = sizeof(clientAddr);
			SOCKET clientSocket = accept(serverSocket, (sockaddr *) &clientAddr, &addrSize);
			if (clientSocket == INVALID_SOCKET)
			{
				printf("accept failed\n");
				continue;
			}

			// CHECKS FOR MAX CONNECTIONS
			if (conns.size() == 2)
			{
				closesocket(clientSocket);
				printf("MAX CONNECTIONS\n");
			}
			else
			{
				// Create a new Connection object, and add it to the collection.
				conns.push_back(new Connection(clientSocket));
			}
		}

		// Check each of the clients.
		for (auto it = conns.begin(); it != conns.end(); )  // note no ++it here
		{
			Connection *conn = *it;
			bool dead = false;

			// Is there data to read from this client's socket?
			if (FD_ISSET(conn->sock(), &readable))
			{
				dead |= conn->doRead();
				conn->doWrite();

				//if (FD_ISSET(conn->sock(), &writeable))
				//{
				//}
			}

			if (dead)
			{
				// The client said it was dead -- so free the object,
				// and remove it from the conns list.
				delete conn;
				it = conns.erase(it);
			}
			else
			{
				++it;
			}
		}
	}

	// We won't actually get here, but if we did then we'd want to clean up...
	printf("Quitting\n");
	closesocket(serverSocket);
	WSACleanup();
	return 0;
}
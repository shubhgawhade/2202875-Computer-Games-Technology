/* The Connection class */

#include <cstdio>
#include <WinSock2.h>

#include "connection.h"

#pragma comment(lib, "ws2_32.lib")


// Constructor.
// sock: the socket that we've accepted the client connection on.
Connection::Connection(SOCKET sock)
	: sock_(sock), readCount_(0)
{
	printf("New connection\n");
}

// Destructor.
Connection::~Connection()
{
	printf("Closing connection\n");
	closesocket(sock_);
}

// Return the client's socket.
SOCKET Connection::sock()
{
	return sock_;
}

// Return whether this connection is in a state where we want to try
// reading from the socket.
bool Connection::wantRead()
{
	// At present, we always do.
	return true;
}

bool Connection::wantWrite()
{
	// At present, we always do.
	return true;
}

// Call this when the socket is ready to read.
bool Connection::doRead()
{
	// Receive as much data from the client as will fit in the buffer.
	int spaceLeft = (sizeof readBuffer_) - readCount_;
	int count = recv(sock_, readBuffer_ + readCount_, spaceLeft, 0);
	if (count <= 0) {
		printf("Client connection closed or broken\n");
		return true;
	}

	// We've successfully read some more data into the buffer.
	readCount_ += count;

	if (readCount_ < MESSAGESIZE) {
		// ... but we've not received a complete message yet.
		// So we can't do anything until we receive some more.
		return false;
	}

	// We've got a complete message.
	printf("Received message from the client: '");
	fwrite(readBuffer_, 1, MESSAGESIZE, stdout);
	printf("'\n");

	if (memcmp(readBuffer_, "quit", 4) == 0) {
		printf("Client asked to quit\n");
		return true;
	}

	// Clear the buffer, ready for the next message.
	readCount_ = 0;

	return false;
}

bool Connection::doWrite()
{
	// Send the same data back to the client.
	// FIXME: the socket might not be ready to send yet -- so this could block!
	// FIXME: and we might not be able to write the entire message in one go...
	int count = send(sock_, readBuffer_, MESSAGESIZE, 0);
	if (count <= 0) {
		printf("Client connection closed or broken\n");
		return true;
	}
	else if (count != MESSAGESIZE)
	{
		printf("send failed\n");
		return true;
	}

	return false;
}

#include <iostream>
#include <string.h>
#include <SFML/Network.hpp>

// ----- The server -----

int main()
{
	// Create a socket and bind it to the port 55002
	sf::UdpSocket socket;
	socket.bind(55002);

	// Receive a message from anyone
	char buffer[1024];
	std::size_t received = 0;
	sf::IpAddress sender;
	unsigned short port;
	socket.receive(buffer, sizeof(buffer), received, sender, port);
	std::cout << sender << " said: " << buffer << std::endl;

	// Send an answer
	std::string message = "Welcome " + sender.toString();
	socket.send(message.c_str(), message.size() + 1, sender, port);
}
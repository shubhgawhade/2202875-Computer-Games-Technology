#include <iostream>
#include <string.h>
#include <SFML/Network.hpp>

// ----- The client -----

int main()
{
	// Create a socket and bind it to the port 55001
	sf::UdpSocket socket;
	socket.bind(55001);

	// Send a message to 192.168.1.50 on port 55002
	//std::string message = "Hi, I am " + sf::IpAddress::getLocalAddress().toString();
	//socket.send(message.c_str(), message.size() + 1, "192.168.0.122", 55002);

	// Receive an answer (most likely from 192.168.1.50, but could be anyone else)
	//char* buffer = new char[1024];
	//std::size_t received = 0;
	//sf::IpAddress sender;
	//unsigned short port;
	//socket.receive(buffer, sizeof(buffer), received, sender, port);
	//std::cout << sender << " said: " << buffer << std::endl;

	std::size_t received = 0;
	sf::IpAddress sender;
	unsigned short port;
	char buffer[1024];
	
	while (true)
	{
		printf("Type some text (\"quit\" to exit): ");
		fflush(stdout);

		// Read a line of text from the user.a
		std::string line;
		std::getline(std::cin, line);

		socket.send(line.c_str(), line.size() + 1, "127.0.0.1", 55002);

		socket.receive(buffer, sizeof(buffer), received, sender, port);
		std::cout << sender << " said: " << buffer << std::endl;
	}
}
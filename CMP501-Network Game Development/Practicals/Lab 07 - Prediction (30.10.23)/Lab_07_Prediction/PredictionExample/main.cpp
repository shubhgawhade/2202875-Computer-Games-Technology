#include <SFML\Graphics.hpp>
#include <sstream>
#include <iomanip>
#include "Tank.h"
#include "NetworkSimulator.h"
#include "TankMessage.h"

//Rounds a float to two decimal places and turns it into a string
std::string Stringify( float value ) {
	std::stringstream sStream;
	sStream << std::fixed << std::setprecision( 2 ) << value;
	std::string t;
	sStream >> t;
	return t;
}

int main() {
	sf::RenderWindow window(sf::VideoMode(640, 480), "CMP303 - Prediction");
	window.setFramerateLimit(60);	//Request 60 frames per second
	
	//Create two tanks (Can also accept "black" and "red")
	Tank tanks[2]{ Tank("green"), Tank("blue") };

	tanks[0].setPosition(64, 256);

	tanks[1].setPosition(416, 128);
	tanks[1].SetRenderMode(Tank::RenderMode::REAL_AND_PREDICTED);

	//Initialise the background texture and sprite
	sf::Texture floorTexture;
	sf::Sprite floor;
	floorTexture.loadFromFile("Assets/tileSand1.png");
	floorTexture.setRepeated(true);
	floor.setTexture(floorTexture);
	floor.setTextureRect(sf::IntRect(0, 0, 640, 480));

	//Initialise font and text
	sf::Font montserrat;
	sf::Text debugText;
	montserrat.loadFromFile("Assets/Montserrat-Regular.ttf");
	debugText.setFont(montserrat);
	debugText.setOutlineColor(sf::Color::Black);
	debugText.setOutlineThickness(1.f);

	//Clock for timing the 'dt' value
	sf::Clock clock;
	float sendRate	= 0.5f;
	float latency	= 0.3f;
	float gameSpeed = 1.0f;
	float startTime = sendRate * 3.0f;

	//When are we next printing the predicted position (so we don't spam the console)
	float nextPrint = startTime;

	//Create a network simulator with that "sends" a message every 0.5 seconds and has a latency of 0.1 seconds
	NetworkSimulator netSimulator(sendRate, latency);
	netSimulator.m_MyID = 0;	//On the network, we are Tank 0
	
	while (window.isOpen()) {
		//Get the time since the last frame in milliseconds
		float dt = clock.restart().asSeconds() * gameSpeed;

		sf::Event event;
		while (window.pollEvent(event))	{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Key::Escape)
					window.close();
				if( event.key.code == sf::Keyboard::Key::R ) {
					tanks[0].Reset(); tanks[1].Reset();
					netSimulator.Reset();
					nextPrint = startTime;
					printf( "\n\n--------RESET--------\n\n" );
				}
			}
		}

		//If we're at the start, just advance the time by 3.5 seconds, so we have a few packets in the queue already
		if( netSimulator.Time() < 1.0f ) {
			printf( "BEGIN SIMULATION\n" );
			netSimulator.Update(startTime);
		}

		if (netSimulator.Time() < 18.0f) {
			TankMessage msg;

			//Update the network simulation
			netSimulator.Update(dt);
			//Get any 'network' messages that are available
			while (netSimulator.ReceiveMessage(msg)) {
				printf("Received message: ID= %d, Pos = (%.2f, %.2f), Time =%.2f\n", msg.id, msg.x, msg.y, msg.time);
				tanks[msg.id].AddMessage(msg);
			}

			//Update the tanks
			for( int i = 0; i < sizeof( tanks ) / sizeof( Tank ); i++ ) {
				tanks[i].Update( dt );	//Update the real position of the tank with the info from the latest packet
				if( i != netSimulator.m_MyID ) {
					//Get the predicted position of the tank at the current Game Time and move the ghost to that position
					tanks[i].setGhostPosition( tanks[i].RunPrediction( netSimulator.Time() ) );

					if( netSimulator.Time() > nextPrint ) {
						//Get the predicted position of the tank at a specific interval and print it to the console
						sf::Vector2f predictedPosition = tanks[i].RunPrediction( nextPrint );
						printf( "\tPredicted positiion:  (%.2f, %.2f), Time =%.2f\n", predictedPosition.x, predictedPosition.y, nextPrint );
						nextPrint = nextPrint + (sendRate * 0.25f);	//Print 4 times per packet
					}
				}
			}
		}
		debugText.setString( "Game Time: " + Stringify( netSimulator.Time() ));

		//Render the scene
		window.clear();
		window.draw(floor);
		for (auto& tank : tanks) {
			tank.Render(&window);
		}
		window.draw(debugText);
		window.display();		
	}

	return 0;
}
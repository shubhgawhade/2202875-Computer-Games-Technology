#include <iostream>		
#include <thread>	 
#include <chrono>
#include <conio.h>

using namespace std;			// cout, endl
using namespace this_thread;	// sleep_for
using namespace chrono;			// seconds, milliseconds

int main()
{
	bool program_running = true;

	std::cout << "Starting Finite State Machine. Press ESC key to close." << endl;

	enum class EmotionalModel 
	{
		CALM, 
		ALERT, 
		AGITATED, 
		SURPRISED, 
		HAPPY, 
		ANGRY,
		NONE
	};

	EmotionalModel npcEmotionalState = EmotionalModel::CALM;
	//EmotionalModel previousState = EmotionalModel::NONE;

	bool hearsNoise = false;
	bool seesSomeone = false;
	bool friendly = false;
	bool aggressive = false;
	bool jokes = false;
	bool attacks = false;
	bool movesAway= false;

	do {

		/*

		Write your code here for the finite state machine example

		*/

		//if (previousState != npcEmotionalState)
		{
			switch (npcEmotionalState)
			{
			case EmotionalModel::CALM:

				printf("NPC is CALM\n");

				break;


			case EmotionalModel::ALERT:

				printf("NPC is ALERT\n");
				
				break;


			case EmotionalModel::AGITATED:

				printf("NPC is AGITATED\n");

				break;


			case EmotionalModel::SURPRISED:

				printf("NPC is SURPRISED\n");

				break;


			case EmotionalModel::HAPPY:

				printf("NPC is HAPPY\n");

				break;


			case EmotionalModel::ANGRY:

				printf("NPC is ANGRY\n");

				break;
			}

			//previousState = npcEmotionalState;
		}

		// Sleep the current thread for 1000 milliseconds. Can be repalce with seconds(1)
		this_thread::sleep_for(milliseconds(1000));

		if (_kbhit())
		{
			char input_char = _getch();

			// Check for ESC key. See table here http://www.asciitable.com/
			if (input_char == 27)
			{
				program_running = false;
			}

			switch (std::toupper(input_char))
			{
			case 78: // NPC HEARS NOISE ('N')
			
				npcEmotionalState = EmotionalModel::ALERT;
			
				break;
			}
		}

	} while (program_running);

	std::cout << "Ending Finite State Machine" << endl;

	return 0;
}
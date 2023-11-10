#include <array>
#include <utility>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;

#define ACTION_NUM 3
#define STATE_NUM 3

enum State { STABLE = 0, TOO_MUCH_PRESSURE, TOO_LITTLE_PRESSURE };
enum Action { WAIT = 0, ADD_WATER, RELEASE_WATER };
std::array<std::array<float, ACTION_NUM>, STATE_NUM> Q = { {0} };
std::array<std::string, ACTION_NUM> action_to_string = { "WAIT", "ADD WATER", "RELEASE WATER" };
std::array<std::string, ACTION_NUM> state_to_string = { "STABLE", "TOO MUCH PRESSURE", "TOO LITTLE PRESSURE" };


void printMatrix()
{
	using namespace std;
	cout << "Q Matrix" << endl << endl;
	for (int i = 0; i < STATE_NUM; i++)
	{
		for (int j = 0; j < ACTION_NUM; j++)
		{
			cout << "For state \"" << state_to_string[i] << "\" the action \"" << action_to_string[j] << "\" has a value of: " << Q[i][j] << endl;
		}
	}
}

int main()
{
	srand(time(NULL));

	float system_pressure = 50.f;

	int iterator = 0;

	while (system_pressure > 0)
	{
		std::cout << "System pressure is sitting at: " << system_pressure << "." << std::endl;

		// Describe State
		// TODO: Store a local variable that describes the current state

		// Choose Action
		// TODO: Store a local variable that explains what action the AI will perform

		// Perform the Action
		// TODO: Implement the chosen action. Use a switch statement or function to manage this.

		// Calculate Reward
		// TODO: Calculate the reward you want to give to the AI

		// Update Q Matrix
		// TODO: Update the correct item the Q matrix with the correct reward.

		// TODO: Print out which action the AI ran. You can use a similar method to the action_to_string or state_to_string described in the printMatrix function

		switch (chosen_action)
		{
		case Action::WAIT:
			// TODO: Add code
			break;
		case Action::ADD_WATER:
			// TODO: Add code
			break;
		case Action::RELEASE_WATER:
			// TODO: Add code
			break;
		default:
			// We should never reach this point
			// but its good to prepare for it just in case
			break;
		}
		
		system_pressure -= (rand() % 4);
		std::cout << "System has lost some pressure." << std::endl;
		std::this_thread::sleep_for(500ms);
		iterator++;
	}

	printMatrix();

	std::cin.get();
}
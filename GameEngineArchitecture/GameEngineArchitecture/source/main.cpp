#define SDL_MAIN_HANDLED
#include "GameEngine.h"

#include <sstream>

using namespace GE;

int main(int argc, char* argv[])
{
	// Create object for game engine
	GameEngine gameEngine;

	// Initialise game engine
	if (!gameEngine.Init(true))
	{
		DisplayInfoMessage("Failed to initialise game engine");

		return -1;
	}

	// Store the time at two points inthe program
	Uint32 lastTime = SDL_GetTicks(), currentTime = 0;

	// Frame counter
	int frameCount = 0;

	// Loop to keep running
	while (gameEngine.KeepRunning())
	{
		// Update the state of the game
		gameEngine.Update();

		// Draw onto the screen
		gameEngine.Draw();

		
		/*
		
		// OLD FRAME COUNTER

		// One frame completed so update the frame counter
		frameCount++;

		// Get the current time
		currentTime = SDL_GetTicks();

		// Is the difference between the two times 1000ms
		if (currentTime - lastTime >= 1000)
		{
			// Create a variable to store a string
			std::ostringstream msg;

			// Create a message based on the value in frameCount
			msg << "FPS : " << frameCount;

			// Display message in window title
			gameEngine.SetWindowTitle(msg.str().c_str());

			// Reset the frame count
			frameCount = 0;

			// Update the last time for the next count
			lastTime = currentTime;
		}

		*/
	}

	// Destroy the game engine
	gameEngine.Shutdown();

	return 0;
}
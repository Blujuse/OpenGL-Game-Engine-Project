#pragma once
#include <SDL.h>
#include <iostream>
#include "Camera.h"

namespace GE
{

	class FPSCameraController
	{
	public:
		FPSCameraController(Camera* cam, const float mouseSensitivity, const float moveDistance)
		{
			// Camera object controlled by the controller
			theCam = cam;

			// Time used to determine move step
			oldTicks = SDL_GetTicks();

			// Get the window dimensions in order to determine centre
			SDL_Window* window = SDL_GL_GetCurrentWindow();

			mouseSens = mouseSensitivity;
			moveDist = moveDistance;
			sidewaysSpeedMultiplier = 100.0f;

			int w, h;

			SDL_GetWindowSize(window, &w, &h);
			// Set mouse to middle of window from which to work out
			// a deflection
			SDL_WarpMouseInWindow(window, w / 2, h / 2);
		}

		// Destructor
		virtual ~FPSCameraController() {}

		void update() 
		{
			// Get time now
			Uint32 newTicks = SDL_GetTicks();

			// Calculate elapsed time for determining move amount later
			Uint32 diffTicks = newTicks - oldTicks;

			// Get window width and height in order to work out midpoint
			// used to determine deflection
			SDL_Window* window = SDL_GL_GetCurrentWindow();

			int w, h;

			SDL_GetWindowSize(window, &w, &h);

			// Get current mouse x and y
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			// Calculate the deflection, inverse on y as this is flipped
			float diffX = mouseX - (w / 2);
			float diffY = (h / 2) - mouseY;

			// Update the yaw and pitch based on the mouse deflection
			theCam->setYaw(theCam->getYaw() + diffX * mouseSens);
			theCam->setPitch(theCam->getPitch() + diffY * mouseSens);

			// Read keyboard directly
			const Uint8* keyState = SDL_GetKeyboardState(nullptr);

			// Move in direction based on time elapsed since last update
			if (keyState[SDL_SCANCODE_W]) 
			{
				theCam->setPos(theCam->getPos() + theCam->getLookDir() * (moveDist * diffTicks / 1000.0f));
				//std::cout << "W PRESSED" << std::endl;
			}
			if (keyState[SDL_SCANCODE_S])
			{
				theCam->setPos(theCam->getPos() - theCam->getLookDir() * (moveDist * diffTicks / 1000.0f));
				//std::cout << "S PRESSED" << std::endl;
			}
			if (keyState[SDL_SCANCODE_A])
			{
				theCam->setPos(theCam->getPos() - glm::normalize(glm::cross(theCam->getLookDir(), theCam->getUpDir())) * ((moveDist * sidewaysSpeedMultiplier) * diffTicks / 1000.0f));
				//std::cout << "A PRESSED" << std::endl;
			}
			if (keyState[SDL_SCANCODE_D])
			{
				theCam->setPos(theCam->getPos() + glm::normalize(glm::cross(theCam->getLookDir(), theCam->getUpDir())) * ((moveDist * sidewaysSpeedMultiplier) * diffTicks / 1000.0f));
				//std::cout << "D PRESSED" << std::endl;
			}

			// Quit if escape key pressed, I've got this here as it makes sense since I already have the keyboard state and stuff setup
			if (keyState[SDL_SCANCODE_ESCAPE])
			{
				SDL_Quit();
			}

			// Get half width and height of window
			float wHalf = w / 2;
			float hHalf = h / 2;
			//std::cout << "wHalf: " << wHalf << " hHalf: " << hHalf << std::endl;
			//std::cout << "mouseX: " << mouseX << " mouseY: " << mouseY << std::endl;

			// Set mouse to middle of window
			SDL_WarpMouseInWindow(SDL_GL_GetCurrentWindow(), wHalf, hHalf);
			SDL_ShowCursor(SDL_DISABLE);

			oldTicks = newTicks;
		}


	private:

		Camera* theCam;
		Uint32 oldTicks;

		float mouseSens;
		float moveDist;
		float sidewaysSpeedMultiplier;
	};
}
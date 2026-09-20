#include "Texture.h"

namespace GE
{
	void Texture::loadTexture(std::string filename)
	{
		// Load texture data from file
		SDL_Surface* surfaceImage = IMG_Load(filename.c_str());

		// Check it was loaded okay
		if (surfaceImage == nullptr)
		{
			return;
		}

		// Get dimensions, needed for opengl
		width = surfaceImage->w;
		height = surfaceImage->h;

		// Get the format the SDL object, will be in SDL image's format
		format = surfaceImage->format->format;

		// Determine opengl format from SDL format
		switch (format)
		{
			// RGBA where there are 8 bits per pixel, 32 bits in total
			case SDL_PIXELFORMAT_RGBA32: format = GL_RGBA;
				break;

			// RGB where there are 8 bits per pixel, 24 bits in total
			case SDL_PIXELFORMAT_RGB24: format = GL_RGB;
				break;

			// More formats could be included here, there are alternate
			// ways to achieve in the same mapping

			// Default is RGB but should really use as many case as expected texture data formats
			default:
				format = GL_RGB;
			break;
		}

		// Create a texture name for the texture
		glGenTextures(1, &textureName);

		// Select created texture for subsequent texture operations, to setup the texture for opengl
		glBindTexture(GL_TEXTURE_2D, textureName);

		// Copy the pixel data from the SDL_Sruface object to the opengl texture
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, surfaceImage->pixels);

		// Configure how the texture will be manipulated when it needs to be
		// reduced or increased when rendering
		// GL_LINEAR is weighted average of the colours around the texture coords
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Release the surface object and memory associated with it as no longer need it
		SDL_FreeSurface(surfaceImage);

		return;
	}
}

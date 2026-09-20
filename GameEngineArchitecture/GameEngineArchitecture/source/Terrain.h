#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <string>

namespace GE
{
	class Terrain
	{
	public:
		Terrain(std::string heightMapFilename, float passthroughHeight, float passthroughScale);

		// TERRAIN SCALE AND HEIGHTMAP ARE CURRENTLY HARDCODED
		// MODIFY TO MAKE CONFIGURABLE AT RUNTIME AT A LATER DATE

		// getVertices() returns the number of vertices in the terrain
		GLuint getVertices()
		{
			return vbo;
		}

		// getIndices() returns the index buffer object as need to bind to the pipeline
		// to render using index buffer
		GLuint getIndices()
		{
			return ibo;
		}

		// getIndexCount() returns the number of indices in the index buffer object
		GLuint getIndexCount()
		{
			return indexCount;
		}

	private:

		GLuint vbo, ibo, indexCount;
	};
}
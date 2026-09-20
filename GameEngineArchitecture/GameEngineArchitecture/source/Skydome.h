#pragma once
#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>

namespace GE
{
	class Skydome
	{
	public:
		Skydome();
		~Skydome();

		// Returns number of vertices for index buffer
		GLuint getVertices() { return vbo; }

		// Gets the buffer object as needed to bind to pipeline in
		// order to render using index buffer
		GLuint getIndices() { return ibo; }

		// Returns the number of indices in the index buffer
		GLuint getIndexCount() { return indexCount; }
	private:
		GLuint vbo, ibo, indexCount;
	};
}
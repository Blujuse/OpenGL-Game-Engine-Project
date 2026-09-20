#pragma once

#include <GL/glew.h>

namespace GE
{
	class Model
	{
	public:

		// Constructor
		Model()
		{
			vbo = 0;
			numVertices = 0;
		}

		~Model()
		{
			glDeleteBuffers(1, &vbo);
		}

		bool LocalLoadFromFile(const char* filename, int usingNormals);

		bool LoadFromFile(const char* filename);

		bool LoadFromFileNoNorm(const char* filename);

		// Returns the number of vertices to create a vertex buffer
		// Based on model vertices
		GLuint GetVertices()
		{
			return vbo;
		}

		// Returns the number of vertices in model, need this for rendering as OpenGL
		// needs to know how many vertices to render
		int GetNumVertices()
		{
			return numVertices;
		}

	private:

		// This stores the triangle vertex buffer object containing the vertices
		// transferred from this code to the graphics memory
		GLuint vbo;

		// Number of vertices in the model
		int numVertices;
	};
}
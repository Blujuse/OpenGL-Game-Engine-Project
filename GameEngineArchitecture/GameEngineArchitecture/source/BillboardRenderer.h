#pragma once
// This class requires you've implemented FPS camara
#include "Camera.h"
#include "Billboard.h"

namespace GE {
	class BillboardRenderer
	{
	public:
		BillboardRenderer();

		~BillboardRenderer() 
		{
			glDeleteBuffers(1, &vboQuad);
		}

		void init();

		void update();

		void draw(Billboard* b, Camera *);

	private:
		// Member fields
		// This member stores the program object that contains the shaders
		GLuint programId;

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexLocation;

		// Link to vColour attribute which receives a colour
		// and passes to fColour for fragment shader
		GLint vertexUVLocation;

		// This member stores the triangle vertex buffer object containing the vertices
		// transferred from this code to the graphics memory
		GLuint vboQuad;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;

		// Fog uniform link variables
		GLuint fogColourId;
		GLuint fogDensityId;
	};
}

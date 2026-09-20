#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm.hpp>
#include <vector>
#include "Camera.h"


namespace GE {
	class BatchRenderer
	{
	public:
		BatchRenderer() {
			numModels = 0;
			vbo = -1;
		}

		virtual ~BatchRenderer() {
			// 16. Release the textures.  Because the textures were stored
			// in a vector just need to pass the number of items in the vector
			// and the vector data
			glDeleteTextures((GLsizei) textures.size(), textures.data());
			glDeleteBuffers(1, &vbo);
			glDeleteProgram(programId);
		}

		void init();

		bool addModelToBatch(const char* modelFilename, const char* textureFilename, glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

		void draw(Camera *cam);

	private:
		GLuint loadTexture(const char *textureFilename);

	private:
		// Single vertex buffer for the batch
		GLuint vbo;

		// 1. Vectors for the textures
		std::vector<GLuint> textures;
		std::vector<GLint> textureUnits;

		// 2. Stores the number of models in the batch
		int numModels;

		// 3. Stores the total number of vertices in the batch
		// need this value for rendering the batch
		int totalVertices;

		GLuint programId;

		// This member stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexPos3DLocation;

		// Link to vColour attribute which receives the colour (rgba) value // in a Vertex
		GLint vertexUVLocation;

		// Linto to vertex texture id for selecting the texture associated with vertex
		GLuint vertexTextureId;

		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint transformUniformId;
		GLuint samplerId;
	};
}
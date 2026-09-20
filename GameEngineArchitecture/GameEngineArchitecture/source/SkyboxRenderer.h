#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include "Camera.h"

namespace GE
{
	class SkyboxRenderer
	{
	public:
		SkyboxRenderer(std::string frontFilename, std::string backFilename,
			std::string rightFilename, std::string leftFilename,
			std::string topFilename, std::string bottomFilename)
		{
			std::vector<std::string> filenames;

			filenames.push_back(rightFilename);
			filenames.push_back(leftFilename);
			filenames.push_back(topFilename);
			filenames.push_back(bottomFilename);
			filenames.push_back(frontFilename);
			filenames.push_back(backFilename);

			createCubemap(filenames);
			createCubeVBO();
			createSkyboxProgram();
		}

		~SkyboxRenderer() {}

		void draw(Camera* cam);

		void destroy();

	private:

		void createCubemap(std::vector<std::string> filenames);
		void createCubeVBO();
		void createSkyboxProgram();

	private:

		GLuint skyboxCubeMapName;

		// Stores the program object that contains the shaders
		GLuint skyboxProgramId;

		// Stores the attribute to select into the pipeline to link the triangle vertices
		GLuint vertexLocation;

		// Stores the triangle vertex buffer object containing the vertices 
		// transferred from this code to the graphics memory
		GLuint vboSkybox;

		//GLSL uniform variables for the transformation, view and projection matrices
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;
	};
}
#include <SDL_image.h>
#include <iostream>
#include <gtc/type_ptr.hpp>

#include "SkyboxRenderer.h"
#include "ShaderUtils.h"

namespace GE
{
	// Cube for skybox has its own vertex struct
	// there is no colour or uv	this is because 
	// cubemap is sampled from cube vertices

	struct CubeVertex
	{
		float x, y, z;
		
		CubeVertex()
		{
			x = 0.0f;
			y = 0.0f;
			z = 0.0f;
		}

		CubeVertex(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}
	};

	const float SIDE = 5.0f;

	CubeVertex cube[] = {
		// Front face
		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),

		// Back face
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(SIDE, -SIDE, SIDE),

		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE, SIDE, SIDE),

		// Left face
		CubeVertex(-SIDE, -SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, SIDE),
		CubeVertex(-SIDE,  SIDE, -SIDE),

		CubeVertex(-SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),

		// Right face
		CubeVertex(SIDE, -SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, SIDE),
		CubeVertex(SIDE,  SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),

		// Top face
		CubeVertex(-SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE,  SIDE),
		CubeVertex(SIDE, SIDE, -SIDE),

		CubeVertex(SIDE,  SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE, -SIDE),
		CubeVertex(-SIDE, SIDE,  SIDE),

		// Bottom face
		CubeVertex(-SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE,  SIDE),
		CubeVertex(SIDE, -SIDE, -SIDE),

		CubeVertex(SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE, -SIDE),
		CubeVertex(-SIDE, -SIDE,  SIDE),
	};

	void SkyboxRenderer::createCubemap(std::vector<std::string> filenames)
	{
		// Name for cubmap
		glGenTextures(1, &skyboxCubeMapName);

		// Select the cubemap as the target to subsequent texture operations
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		// Load texture data from files into the cubemap
		for (int faceNum = 0; faceNum < 6; faceNum++)
		{
			// Load texture data from file
			SDL_Surface* surfaceImage = IMG_Load(filenames[faceNum].c_str());

			// Check if the image was loaded
			if (surfaceImage == nullptr)
			{
				std::cerr << "Failed to load image: " << filenames[faceNum] << std::endl;
				return;
			}

			// Get the format of the image from the SDL object
			GLenum format = surfaceImage->format->format;

			// Determine OpenGL format from SDL format
			switch (format)
			{
			case SDL_PIXELFORMAT_RGBA32:
				format = GL_RGBA;
				break;

			case SDL_PIXELFORMAT_RGB24:
				format = GL_RGB;
				break;

			default:
				format = GL_RGB;
				break;
			}

			// Copy the pixel data from the SDL_Surface object to the OpenGL texture
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceNum, 0, 
				format, surfaceImage->w, surfaceImage->h, 
				0, 
				format, 
				GL_UNSIGNED_BYTE, 
				surfaceImage->pixels);

			// Release the SDL_Surface object and its memory, not needed anymore
			SDL_FreeSurface(surfaceImage);
		}

		// Configure how the texture will be manipulated when it needs to be reduced or increased
		// when rendering onto an object. GL_LINEAR is a weighted average of the colours around the texture coords
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Clamp the text edge to deal with inaccuracies, if the xyz coords go over under 0/1
		// Then the sampler will wrap a texture resulting in artefacting at the edges
		// Sets the cubemap to use clamping to repeat the last line of the texture
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

	void SkyboxRenderer::createCubeVBO()
	{
		// Create the buffer
		glGenBuffers(1, &vboSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		// Copy vertex data from vector to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		// Release from pipeline
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SkyboxRenderer::createSkyboxProgram()
	{
		// Shader code
		const GLchar* V_ShaderCode[] = {
			"#version 410\n"
			"in vec3 vertexPos3D;\n"
			"out vec3 texCoord;\n"
			"uniform mat4 viewMat;\n"
			"uniform mat4 projMat;\n"
			"void main() {\n"
			"vec4 v = vec4(vertexPos3D.xyz, 1);\n"
			"v = projMat * viewMat * v;\n"
			"gl_Position = v;\n"
			"texCoord = vertexPos3D;\n"
			"}\n"
		};

		// Fragment shader code
		const GLchar* F_ShaderCode[] = {
			"#version 410\n"
			"in vec3 texCoord;\n"
			"uniform samplerCube sampler;\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = vec4(texture(sampler, texCoord).rgb, 1.0f);\n"
			"}\n"
		};

		bool result = compileProgram(V_ShaderCode, F_ShaderCode, &skyboxProgramId);

		// Check
		if (!result)
		{
			std::cerr << "Failed to create skybox program" << std::endl;
			return;
		}

		// Now get a link to vertexPos3D so we can link the attribute
		// to the vertecies when rendering
		vertexLocation = glGetAttribLocation(skyboxProgramId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1)
		{
			std::cerr << "Failed to get vertexPos3D location" << std::endl;
			return;
		}

		// Link the uniforms to the member fields
		viewUniformId = glGetUniformLocation(skyboxProgramId, "viewMat");
		projectionUniformId = glGetUniformLocation(skyboxProgramId, "projMat");
		samplerId = glGetUniformLocation(skyboxProgramId, "sampler");
	}

	void SkyboxRenderer::draw(Camera* cam)
	{
		bool isDepthTestEnabled = glIsEnabled(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);

		glm::mat4 camView = cam->getViewMatrix();
		glm::mat4 camProj = cam->getProjectionMatrix();

		// Zero out the translation as only want rotation to make sure it is orientated
		camView[3][0] = 0.0f;
		camView[3][1] = 0.0f;
		camView[3][2] = 0.0f;

		// Slect the program into the rendering context
		glUseProgram(skyboxProgramId);

		// Select the program into the rendering context
		glBindBuffer(GL_ARRAY_BUFFER, vboSkybox);

		// Set the uniforms in the shader
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(camView));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(camProj));

		// Enable the attribute to be passed from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexLocation attribute
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(CubeVertex), (void*)offsetof(CubeVertex, x));

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxCubeMapName);

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, sizeof(cube) / sizeof(CubeVertex));

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexLocation);

		// Unselect the program from the context
		glUseProgram(0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (isDepthTestEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
	}

	void SkyboxRenderer::destroy()
	{
		glDeleteProgram(skyboxProgramId);

		glDeleteBuffers(1, &vboSkybox);

		glDeleteTextures(1, &skyboxCubeMapName);
	}
}
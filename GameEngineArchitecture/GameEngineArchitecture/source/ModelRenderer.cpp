#include <iostream>
#include <gtc/type_ptr.hpp>

#include "ModelRenderer.h"
#include "Vertex.h"
#include "ShaderUtils.h"

//
//
// APOLIGIES TO WHO EVER HAS TO READ THE "LOCAL" FUNCTIONS THEY ARE A NIGHTMARE TO READ
// QUITE POSSIBLY THE WORST CODE I HAVE EVER WRITTEN
//
//

namespace GE
{
	// Constructor, initialises transformation variables
	ModelRenderer::ModelRenderer()
	{
		// Initialise the transformation variables
		posX = posY = posZ = 0.0f;
		rotX = rotY = rotZ = 0.0f;
		scaleX = scaleY = scaleZ = 1.0f;

		programId = 0;

		vertexPos3DLocation = 0;

		vertexUVLocation = 0;

		transformUniformId = 0;
		viewUniformId = 0;
		projectionUniformId = 0;
	}

	ModelRenderer::~ModelRenderer()
	{
	}

	enum WhatToDraw
	{
		NORMALS = 1,
		NONORMALS,
		TERRAIN,
		WATER,
		SKYBOX
	};

	// Each init function calls this and uses the enum to know what to do
	void ModelRenderer::LocalInit(int whatToDraw)
	{
		// Create vertex shader first
		// Order does not matter but shaders must be created before the program
		GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
		std::string v_shader_source;

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			// Loading vertex shader from file
			v_shader_source = loadShaderSourceCode("./assets/code/model.vert");
		}
		else if (whatToDraw == WATER)
		{
			v_shader_source = loadShaderSourceCode("./assets/code/water.vert");
		}

		const GLchar* V_ShaderCode[] = { v_shader_source.c_str() };

		// Copy the source to OpenGL for compilation
		glShaderSource(vertexShaderId, 1, V_ShaderCode, NULL);

		// Compile the shader
		glCompileShader(vertexShaderId);

		// Check for compilation errors
		GLint isShaderCompiled = GL_FALSE;

		// Get the compilation status
		glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &isShaderCompiled);

		// Has the shader compiled successfully
		if (isShaderCompiled != GL_TRUE)
		{
			// No, display error message
			std::cout << "Vertex shader compilation failed" << std::endl;

			DisplayShaderCompileError(vertexShaderId);

			return;
		}

		// Create the fragment shader
		GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
		std::string f_shader_source;

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			// Loading vertex shader from file
			f_shader_source = loadShaderSourceCode("./assets/code/model.frag");
		}
		else if (whatToDraw == WATER)
		{
			f_shader_source = loadShaderSourceCode("./assets/code/water.frag");
		}

		// Shader source code
		const GLchar* F_ShaderCode[] = { f_shader_source.c_str() };

		// Transfer the shader code
		glShaderSource(fragmentShaderId, 1, F_ShaderCode, NULL);

		// Compile it
		glCompileShader(fragmentShaderId);

		// Check for errors
		isShaderCompiled = GL_FALSE;
		glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &isShaderCompiled);

		// Display error message if not compiled
		if (isShaderCompiled != GL_TRUE)
		{
			std::cout << "Fragment shader compilation failed" << std::endl;

			DisplayShaderCompileError(fragmentShaderId);

			return;
		}

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			// Create the program object
			programId = glCreateProgram();

			// Attach the shaders to the program
			glAttachShader(programId, vertexShaderId);
			glAttachShader(programId, fragmentShaderId);

			// Link the program
			glLinkProgram(programId);

			// Check for errors
			GLint isProgramLinked = GL_FALSE;
			glGetProgramiv(programId, GL_LINK_STATUS, &isProgramLinked);
			if (isProgramLinked != GL_TRUE)
			{
				std::cerr << "Program linking failed" << std::endl;
			}

			// Get a link to the vertexPos3D so we can link the attribute to vertices
			vertexPos3DLocation = glGetAttribLocation(programId, "vertexPos3D");

			// Check for errors
			if (vertexPos3DLocation == -1)
			{
				std::cerr << "Problem getting vertexPos3D" << std::endl;
			}

			// Get a link to the vColour attribute in the vertex shader, indicates where the colour should be inserted
			vertexUVLocation = glGetAttribLocation(programId, "vUV");

			// Check for errors
			if (vertexUVLocation == -1)
			{
				std::cerr << "Problem getting vUV" << std::endl;
			}

			// Get normal stuff from shader
			vertexNormal = glGetAttribLocation(programId, "vertexNormal");
			// Check for errors
			if (vertexNormal == -1)
			{
				std::cerr << "Problem getting vertexNormal" << std::endl;
			}

			// Get uniform id in shader so cpp program can send data
			viewUniformId = glGetUniformLocation(programId, "viewMat");
			projectionUniformId = glGetUniformLocation(programId, "projMat");
			// Transform matrix uniform
			transformUniformId = glGetUniformLocation(programId, "transformMat");
			samplerId = glGetUniformLocation(programId, "sampler");

			// Link the fog uniform variables
			fogColourId = glGetUniformLocation(programId, "fog_Colour");
			fogDensityId = glGetUniformLocation(programId, "fog_Density");

			// Link the view pos stuff
			viewPosId = glGetUniformLocation(programId, "viewPos");

			// Check if normals should be used
			if (NORMALS)
			{
				vertexTangentLocation = glGetAttribLocation(programId, "vertexTangent");
				if (vertexTangentLocation == -1)
				{
					std::cerr << "Problem getting vertexTangent" << std::endl;
				}

				vertexBitangentLocation = glGetAttribLocation(programId, "vertexBitangent");
				if (vertexBitangentLocation == -1)
				{
					std::cerr << "Problem getting vertexBitangent" << std::endl;
				}

				normalMapSamplerId = glGetUniformLocation(programId, "normalMapSampler");
			}

			// Set fog values in shader in init method
			// Fog does not change during the game
			// Constant values for fog
			glUseProgram(programId);
			glUniform1f(fogDensityId, 0.01f);
			glm::vec3 fogColour = glm::vec3(0.5f, 0.5f, 0.5f);
			glUniform3fv(fogColourId, 1, glm::value_ptr(fogColour));

			glUseProgram(0);
		}
		else if (whatToDraw == WATER)
		{
			// Create the program object
			waterProgramId = glCreateProgram();

			// Attach the shaders to the program
			glAttachShader(waterProgramId, vertexShaderId);
			glAttachShader(waterProgramId, fragmentShaderId);

			// Link the program
			glLinkProgram(waterProgramId);

			// Check for errors
			GLint isProgramLinked = GL_FALSE;
			glGetProgramiv(waterProgramId, GL_LINK_STATUS, &isProgramLinked);
			if (isProgramLinked != GL_TRUE)
			{
				std::cerr << "Program linking failed" << std::endl;
			}

			// Get a link to the vertexPos3D so we can link the attribute to vertices
			vertexPos3DLocationWater = glGetAttribLocation(waterProgramId, "vertexPos3D");

			// Check for errors
			if (vertexPos3DLocationWater == -1)
			{
				std::cerr << "Problem getting vertexPos3D" << std::endl;
			}

			// Get a link to the vColour attribute in the vertex shader, indicates where the colour should be inserted
			vertexUVLocationWater = glGetAttribLocation(waterProgramId, "vUV");

			// Check for errors
			if (vertexUVLocationWater == -1)
			{
				std::cerr << "Problem getting vUV" << std::endl;
			}

			// Get uniform id in shader so cpp program can send data
			viewUniformIdWater = glGetUniformLocation(waterProgramId, "viewMat");
			projectionUniformIdWater = glGetUniformLocation(waterProgramId, "projMat");
			// Transform matrix uniform
			transformUniformIdWater = glGetUniformLocation(waterProgramId, "transformMat");
			samplerIdWater = glGetUniformLocation(waterProgramId, "sampler");

			// Link timer
			timeId = glGetUniformLocation(waterProgramId, "time");

			// Link the fog uniform variables
			fogColourIdWater = glGetUniformLocation(waterProgramId, "fog_Colour");
			fogDensityIdWater = glGetUniformLocation(waterProgramId, "fog_Density");

			// Link the view pos stuff
			viewPosIdWater = glGetUniformLocation(waterProgramId, "viewPos");

			// Set fog values in shader in init method
			// Fog does not change during the game
			// Constant values for fog
			glUseProgram(waterProgramId);
			glUniform1f(fogDensityIdWater, 0.01f);
			glm::vec3 fogColour = glm::vec3(0.5f, 0.5f, 0.5f);
			glUniform3fv(fogColourIdWater, 1, glm::value_ptr(fogColour));

			glUseProgram(0);
		}
	}

	// The below init functions are called from the game engine which then use the local init function in this script
	void ModelRenderer::init()
	{
		LocalInit(NORMALS);
	}

	void ModelRenderer::initNoNorm()
	{
		LocalInit(NONORMALS);
	}

	void ModelRenderer::initWater()
	{
		LocalInit(WATER);
	}

	// Each update function calls this and uses the enum to know what to do
	void ModelRenderer::LocalUpdate(int whatToDraw)
	{
		if (whatToDraw == WATER)
		{
			glUseProgram(waterProgramId);

			if (timeId != -1) 
			{
				glUniform1f(timeId, SDL_GetTicks() / 1000.0f);
			}

			glUseProgram(0);
		}
	}

	// The below update functions are called from the game engine which then use the local update function in this script
	void ModelRenderer::UpdateWater()
	{
		LocalUpdate(WATER);
	}

	// Each draw function calls this and uses the enum to know what to do
	void ModelRenderer::LocalDraw(Camera* cam, Model* model, Terrain* terrain, Skydome* skydome, int whatToDraw)
	{
		if (whatToDraw == SKYBOX)
		{
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			glEnable(GL_CULL_FACE);
		}

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		// Calculate the transformation matrix for the object, start with identity matrix
		glm::mat4 transformMat = glm::mat4(1.0f);

		if (whatToDraw == SKYBOX)
		{
			transformMat = glm::translate(transformMat, glm::vec3(cam->getPosX(), cam->getPosY(), cam->getPosZ())); // This should be the first in the sequence
		}
		else
		{
			transformMat = glm::translate(transformMat, glm::vec3(posX, posY, posZ)); // This should be the first in the sequence
			transformMat = glm::rotate(transformMat, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
			transformMat = glm::rotate(transformMat, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
			transformMat = glm::rotate(transformMat, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
			transformMat = glm::scale(transformMat, glm::vec3(scaleX, scaleY, scaleZ)); // This should be the last in the sequence
		}

		// Get the view and projection matrices from the camera
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projMat = cam->getProjectionMatrix();

		if (whatToDraw != WATER)
		{
			// Select the program into the rendering context
			glUseProgram(programId);

			glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformMat));
			glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projMat));
		}
		else if (whatToDraw == WATER)
		{
			glUseProgram(waterProgramId);

			glUniformMatrix4fv(transformUniformIdWater, 1, GL_FALSE, glm::value_ptr(transformMat));
			glUniformMatrix4fv(viewUniformIdWater, 1, GL_FALSE, glm::value_ptr(viewMat));
			glUniformMatrix4fv(projectionUniformIdWater, 1, GL_FALSE, glm::value_ptr(projMat));
		}

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			// Pass the cam pos to frag shader
			glUniform3f(viewPosId, cam->getPosX(), cam->getPosY(), cam->getPosZ());

			// Select the vertex buffer object
			glBindBuffer(GL_ARRAY_BUFFER, model->GetVertices());
		}
		else if (whatToDraw == TERRAIN || whatToDraw == WATER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertices());
		}
		else if (whatToDraw == SKYBOX)
		{
			glBindBuffer(GL_ARRAY_BUFFER, skydome->getVertices());
		}

		if (whatToDraw != WATER)
		{
			// Enable the attribute to be passed from the vertex buffer object
			glEnableVertexAttribArray(vertexPos3DLocation);
		}
		else if (whatToDraw == WATER)
		{
			// Enable the attribute to be passed from the vertex buffer object
			glEnableVertexAttribArray(vertexPos3DLocationWater);
		}


		if (whatToDraw == NORMALS)
		{
			// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
			// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
			glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // Basically same as 0 but offsetof 
			// is used to make the intent clearer
			// Enable attrivute to be passed colours from the vertex buffer object
			glEnableVertexAttribArray(vertexUVLocation);

			// Define the structure of a vertex for OpenGL to select values from vertex buffer
			// and store in vColour attribute
			glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

			glEnableVertexAttribArray(vertexNormal);

			// Where pipeline will find vertex normal in vertex from vertex buffer
			// to put in vertexNormal attribute
			glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, nx));

			// Enable tangent input
			glEnableVertexAttribArray(vertexTangentLocation);

			// Where in the pipeline will find tangent in a vertex in vertex buffer to put in vertexNormal attribute
			glVertexAttribPointer(vertexTangentLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tx));

			glEnableVertexAttribArray(vertexBitangentLocation);

			// Define where the pipeline will find the bitangent in a vertex in vertex buffer to put in vertexNormal attribute
			glVertexAttribPointer(vertexBitangentLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, btx));
		}
		else if (whatToDraw == NONORMALS)
		{
			// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
			// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
			glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, x)); // Basically same as 0 but offsetof 
			// is used to make the intent clearer
			// Enable attrivute to be passed colours from the vertex buffer object
			glEnableVertexAttribArray(vertexUVLocation);

			// Define the structure of a vertex for OpenGL to select values from vertex buffer
			// and store in vColour attribute
			glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, u));

			glEnableVertexAttribArray(vertexNormal);

			// Where pipeline will find vertex normal in vertex from vertex buffer
			// to put in vertexNormal attribute
			glVertexAttribPointer(vertexNormal, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, nx));
		}
		else if (whatToDraw == TERRAIN || whatToDraw == SKYBOX)
		{
			// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
			// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
			glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, x)); // Basically same as 0 but offsetof 
			// is used to make the intent clearer
			// Enable attrivute to be passed colours from the vertex buffer object
			glEnableVertexAttribArray(vertexUVLocation);

			// Define the structure of a vertex for OpenGL to select values from vertex buffer
			// and store in vColour attribute
			glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, u));
		}
		else if (whatToDraw == WATER)
		{
			// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
			// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
			glVertexAttribPointer(vertexPos3DLocationWater, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, x)); // Basically same as 0 but offsetof 
			// is used to make the intent clearer
			// Enable attrivute to be passed colours from the vertex buffer object
			glEnableVertexAttribArray(vertexUVLocationWater);

			// Define the structure of a vertex for OpenGL to select values from vertex buffer
			// and store in vColour attribute
			glVertexAttribPointer(vertexUVLocationWater, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, u));
		}

		if (whatToDraw != WATER)
		{
			// Select the texture
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(samplerId, 0);
			glBindTexture(GL_TEXTURE_2D, tex->getTextureName());
		}
		else if (whatToDraw == WATER)
		{
			// Select the texture
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(samplerIdWater, 0);
			glBindTexture(GL_TEXTURE_2D, tex->getTextureName());
		}

		if (whatToDraw == NORMALS)
		{
			// Load normal map into shader
			glActiveTexture(GL_TEXTURE2);
			glUniform1i(normalMapSamplerId, 2);
			glBindTexture(GL_TEXTURE_2D, normTex->getTextureName());
		}

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			// Draw the triangle
			// Using sizeof for more flexibility, as dividing vertexData by sizeof(Vertex) will give the number of vertices
			// in the array, this allows for more vertices to be added to the array without changing the draw call
			glDrawArrays(GL_TRIANGLES, 0, model->GetNumVertices());
		}
		else if (whatToDraw == TERRAIN || whatToDraw == WATER)
		{
			// Draw the triangle
			// Using sizeof for more flexibility, as dividing vertexData by sizeof(Vertex) will give the number of vertices
			// in the array, this allows for more vertices to be added to the array without changing the draw call
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getIndices());

			glDrawElements(GL_TRIANGLES, terrain->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		}
		else if (whatToDraw == SKYBOX)
		{
			// Bind index buffer to pipeline as using the indices to render terrain from vertex buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skydome->getIndices());

			// Draw Skydome
			glDrawElements(GL_TRIANGLES, skydome->getIndexCount(), GL_UNSIGNED_INT, nullptr);
		}

		if (whatToDraw != WATER)
		{
			// Unselect the attributes from the context
			glDisableVertexAttribArray(vertexPos3DLocation);
			glDisableVertexAttribArray(vertexUVLocation);
		}
		else
		{
			glDisableVertexAttribArray(vertexPos3DLocationWater);
			glDisableVertexAttribArray(vertexUVLocationWater);
		}

		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			glDisableVertexAttribArray(vertexNormal);
		}
		
		if (whatToDraw == NORMALS)
		{
			glDisableVertexAttribArray(vertexTangentLocation);
			glDisableVertexAttribArray(vertexBitangentLocation);
		}

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program
		glUseProgram(0);

		if (whatToDraw == SKYBOX)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
		
		if (whatToDraw == NORMALS || whatToDraw == NONORMALS)
		{
			glDisable(GL_BLEND);
		}
	}

	// The below draw functions are called from the game engine which then use the local init function in this script
	void ModelRenderer::Draw(Camera* cam, Model *model)
	{
		LocalDraw(cam, model, nullptr, nullptr, NORMALS);
	}

	void ModelRenderer::DrawNoNorm(Camera* cam, Model* model)
	{
		LocalDraw(cam, model, nullptr, nullptr, NONORMALS);
	}

	void ModelRenderer::DrawTerrain(Camera* cam, Terrain* terrain)
	{
		LocalDraw(cam, nullptr, terrain, nullptr, TERRAIN);
	}

	void ModelRenderer::DrawWater(Camera* cam, Terrain* terrain)
	{
		LocalDraw(cam, nullptr, terrain, nullptr, WATER);
	}

	void ModelRenderer::DrawSkydome(Camera* cam, Skydome* skydome)
	{
		LocalDraw(cam, nullptr, nullptr, skydome, SKYBOX);
	}

	// Release resources
	void ModelRenderer::Destroy()
	{
		glDeleteProgram(programId);
		glDeleteProgram(waterProgramId);

		glDeleteBuffers(1, &vboTriangle);
	}

	// Helper function to see shader compiler error messages
	void ModelRenderer::DisplayShaderCompileError(GLuint shaderId)
	{
		GLint msgLength = 0; // Get the length of the error message

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &msgLength);

		// Only display message if more than 1 character was returned
		if (msgLength > 1)
		{
			// Create character buffer to store message
			GLchar* msg = new GLchar[msgLength + 1];

			// Get the error message from the OpenGL internal log
			// into the msg buffer
			glGetShaderInfoLog(shaderId, msgLength, nullptr, msg);

			// Display the error message so we can see what the problem is
			std::cout << msg << std::endl;

			// Clean up
			delete[] msg;
		}
	}

#pragma region Gets & Sets

	float ModelRenderer::getPosX()
	{
		return posX;
	}
	float ModelRenderer::getPosY()
	{
		return posY;
	}
	float ModelRenderer::getPosZ()
	{
		return posZ;
	}
	float ModelRenderer::getRotX()
	{
		return rotX;
	}
	float ModelRenderer::getRotY()
	{
		return rotY;
	}
	float ModelRenderer::getRotZ()
	{
		return rotZ;
	}
	float ModelRenderer::getScaleX()
	{
		return scaleX;
	}
	float ModelRenderer::getScaleY()
	{
		return scaleY;
	}
	float ModelRenderer::getScaleZ()
	{
		return scaleZ;
	}
	void ModelRenderer::setPos(float x, float y, float z)
	{
		posX = x;
		posY = y;
		posZ = z;
	}
	void ModelRenderer::setRot(float rX, float rY, float rZ)
	{
		rotX = rX;
		rotY = rY;
		rotZ = rZ;
	}
	void ModelRenderer::setScale(float sX, float sY, float sZ)
	{
		scaleX = sX;
		scaleY = sY;
		scaleZ = sZ;
	}

	// Texture methods
	void ModelRenderer::setTexture(Texture* _tex)
	{
		tex = _tex;
	}

	void ModelRenderer::setNormalTexture(Texture* _normTex)
	{
		normTex = _normTex;
	}

#pragma endregion
};
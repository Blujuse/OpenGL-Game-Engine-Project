#include <iostream>
#include <gtc/type_ptr.hpp>

#include "ModelRenderer.h"
#include "Vertex.h"
#include "ShaderUtils.h"

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

	// Creates and complies the shaders
	void ModelRenderer::init()
	{
		// Create vertex shader first
		// Order does not matter but shaders must be created before the program
		GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);

		// Loading vertex shader
		std::string v_shader_source = loadShaderSourceCode("./assets/code/model.vert");

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

		// Loading fragment shader from file
		std::string f_shader_source = loadShaderSourceCode("./assets/code/model.frag");

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

		// Set fog values in shader in init method
		// Fog does not change during the game
		// Constant values for fog
		glUseProgram(programId);
		glUniform1f(fogDensityId, 0.01f);
		glm::vec3 fogColour = glm::vec3(0.5f, 0.5f, 0.5f);
		glUniform3fv(fogColourId, 1, glm::value_ptr(fogColour));

		glUseProgram(0);
	}

	void ModelRenderer::Update()
	{

	}

	// Renders triangles
	void ModelRenderer::Draw(Camera* cam, Model *model)
	{
		glEnable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Calculate the transformation matrix for the object, start with identity matrix
		glm::mat4 transformMat = glm::mat4(1.0f);

		transformMat = glm::translate(transformMat, glm::vec3(posX, posY, posZ)); // This should be the first in the sequence
		transformMat = glm::rotate(transformMat, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
		transformMat = glm::rotate(transformMat, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMat = glm::rotate(transformMat, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
		transformMat = glm::scale(transformMat, glm::vec3(scaleX, scaleY, scaleZ)); // This should be the last in the sequence

		// Get the view and projection matrices from the camera
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projMat));

		// Pass the cam pos to grag shader
		glUniform3f(viewPosId, cam->getPosX(), cam->getPosY(), cam->getPosZ());

		// Select the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, model->GetVertices());

		// Enable the attribute to be passed from the vertex buffer object
		glEnableVertexAttribArray(vertexPos3DLocation);

		// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
		// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, x)); // Basically same as 0 but offsetof 
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

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		// Draw the triangle
		// Using sizeof for more flexibility, as dividing vertexData by sizeof(Vertex) will give the number of vertices
		// in the array, this allows for more vertices to be added to the array without changing the draw call
		glDrawArrays(GL_TRIANGLES, 0, model->GetNumVertices());

		// Unselect the attributes from the context
		glDisableVertexAttribArray(vertexPos3DLocation);
		glDisableVertexAttribArray(vertexUVLocation);
		glDisableVertexAttribArray(vertexNormal);

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program
		glUseProgram(0);

		glDisable(GL_CULL_FACE);
		glDisable(GL_BLEND);
	}

	void ModelRenderer::DrawTerrain(Camera* cam, Terrain* terrain)
	{
		glEnable(GL_CULL_FACE);

		// Calculate the transformation matrix for the object, start with identity matrix
		glm::mat4 transformMat = glm::mat4(1.0f);

		transformMat = glm::translate(transformMat, glm::vec3(posX, posY, posZ)); // This should be the first in the sequence
		transformMat = glm::rotate(transformMat, glm::radians(rotX), glm::vec3(1.0f, 0.0f, 0.0f));
		transformMat = glm::rotate(transformMat, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
		transformMat = glm::rotate(transformMat, glm::radians(rotZ), glm::vec3(0.0f, 0.0f, 1.0f));
		transformMat = glm::scale(transformMat, glm::vec3(scaleX, scaleY, scaleZ)); // This should be the last in the sequence

		// Get the view and projection matrices from the camera
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projMat));

		// Select the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, terrain->getVertices());

		// Enable the attribute to be passed from the vertex buffer object
		glEnableVertexAttribArray(vertexPos3DLocation);

		// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
		// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // Basically same as 0 but offsetof 
		// is used to make the intent clearer
		// Enable attrivute to be passed colours from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vColour attribute
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		// Draw the triangle
		// Using sizeof for more flexibility, as dividing vertexData by sizeof(Vertex) will give the number of vertices
		// in the array, this allows for more vertices to be added to the array without changing the draw call
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain->getIndices());

		// Draw the terrain
		glDrawElements(GL_TRIANGLES, terrain->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Unselect index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unselect the attributes from the context
		glDisableVertexAttribArray(vertexPos3DLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program
		glUseProgram(0);

		glDisable(GL_CULL_FACE);
	}

	void ModelRenderer::DrawSkydome(Camera* cam, Skydome* skydome)
	{
		glDisable(GL_DEPTH_TEST);

		// Calculate the transformation matrix for the object, start with identity matrix
		glm::mat4 transformMat = glm::mat4(1.0f);

		transformMat = glm::translate(transformMat, glm::vec3(cam->getPosX(), cam->getPosY(), cam->getPosZ())); // This should be the first in the sequence

		// Get the view and projection matrices from the camera
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projMat));

		// Select the vertex buffer object
		glBindBuffer(GL_ARRAY_BUFFER, skydome->getVertices());

		// Enable the attribute to be passed from the vertex buffer object
		glEnableVertexAttribArray(vertexPos3DLocation);

		// Define the structure if a vertex for OpenGL to use and store in vertexPos3DLocation
		// Using sizeof for flexibility, as more values can be added to the vertex struct for things like uvs
		glVertexAttribPointer(vertexPos3DLocation, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x)); // Basically same as 0 but offsetof 
		// is used to make the intent clearer
		// Enable attrivute to be passed colours from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vColour attribute
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, u));

		// Select the texture
		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

		// Bind index buffer to pipeline as using the indices to render terrain from vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skydome->getIndices());

		// Draw Skydome 
		glDrawElements(GL_TRIANGLES, skydome->getIndexCount(), GL_UNSIGNED_INT, nullptr);

		// Unselect index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// Unselect the attributes from the context
		glDisableVertexAttribArray(vertexPos3DLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Unselect the program
		glUseProgram(0);

		glEnable(GL_DEPTH_TEST);
	}

	// Release resources
	void ModelRenderer::Destroy()
	{
		glDeleteProgram(programId);

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

#pragma endregion
};
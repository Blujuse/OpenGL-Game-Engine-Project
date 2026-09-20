#include <glm.hpp>
#include <iostream>
#include "BillboardRenderer.h"
#include "ShaderUtils.h"
#include "Vertex.h"
#include <ext/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

namespace GE {

	// Billboard quad.  Quad is a square so use scale
	// to get correct dimensions.  Is on 0,0,0
	VertexNoNorm billboard[] = {
		VertexNoNorm(0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		VertexNoNorm(-0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		VertexNoNorm(-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),

		VertexNoNorm(-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		VertexNoNorm(0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f),
		VertexNoNorm(0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f)
	};

	BillboardRenderer::BillboardRenderer()
	{

	}

	void BillboardRenderer::init()
	{
		// New! Load shader source from files.  Need the new ShaderUtils files
		std::string v_shader_source = loadShaderSourceCode("./assets/code/billboard.vert");
		std::string f_shader_source = loadShaderSourceCode("./assets/code/billboard.frag");

		// Due to the unique way OpenGL handles shader source, OpenGL expects
		// an array of strings.  In this case, create an array of the
		// loaded source code strings and pass to compileProgram for compilation
		const GLchar* v_source_array[] = { v_shader_source.c_str() };
		const GLchar* f_source_array[] = { f_shader_source.c_str() };

		// Compile shaders into a program
		if (!compileProgram(v_source_array, f_source_array, &programId)) {
			std::cerr << "Problem building billboard program.  Check console log for more information." << std::endl;
		}

		// Now get a link to the vertexPos3D so we can link the attribute
		// to our vertices when rendering
		vertexLocation = glGetAttribLocation(programId, "vertexPos3D");

		// Check for errors
		if (vertexLocation == -1) {
			std::cerr << "Problem getting vertex3DPos" << std::endl;
		}

		// Now link to the vUV attribute
		vertexUVLocation = glGetAttribLocation(programId, "vUV");

		// Check for errors
		if (vertexUVLocation == -1) {
			std::cerr << "Problem getting vUV" << std::endl;
		}

		// Link the uniforms to the member fields
		transformUniformId = glGetUniformLocation(programId, "transformMat");
		viewUniformId = glGetUniformLocation(programId, "viewMat");
		projectionUniformId = glGetUniformLocation(programId, "projMat");
		samplerId = glGetUniformLocation(programId, "sampler");

		// Link the fog uniform variables
		fogColourId = glGetUniformLocation(programId, "fog_Colour");
		fogDensityId = glGetUniformLocation(programId, "fog_Density");
		glUniform1f(fogDensityId, 0.01f);

		// Create the vertex buffer for the quad
		glGenBuffers(1, &vboQuad);
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		// Transfer vertices to graphics memory
		glBufferData(GL_ARRAY_BUFFER, sizeof(billboard), billboard, GL_STATIC_DRAW);

		// Lot of duplication going on here, so think about how you could reduce this
		// duplication.  Don't forget, billboard is a quad so doesn't need a model to
		// be loaded from a file
	}

	void BillboardRenderer::draw(Billboard* b, Camera *cam)
	{
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Calculate the transformation matrix for the object.  Start with the identity matrix
		// No rotation as going to look down the z axis
		glm::mat4 transformationMat = glm::mat4(1.0f);

		// Get the position of the billboard
		glm::vec3 bbPos = glm::vec3(b->getX(), b->getY(), b->getZ());

		// Get the difference between the camera position and the billboard position
		glm::vec3 diff = cam->getPos() - bbPos;

		// Stuff for rotating to camera
		float angle = atan2f(diff.z, diff.x);
		float PIDIV2 = M_PI / 2.0f;
		float angleNew = PIDIV2 - angle;

		// Position in world
		transformationMat = glm::translate(transformationMat, glm::vec3(b->getX(), b->getY(), b->getZ()));

		// Rotation in world
		transformationMat = glm::rotate(transformationMat, angleNew, glm::vec3(0.0f, 1.0f, 0.0f));

		// Scale, only need x and y
		transformationMat = glm::scale(transformationMat, glm::vec3(b->getScaleX(), b->getScaleY(), 0.0f));

		// Render object in the same way as a regular model
		// Get the view and projection matrices
		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		// Select the program into the rendering context
		glUseProgram(programId);

		// Set the uniforms in the shader
		glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat));
		glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat));
		glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexLocation);
		// Select the vertex buffer object into the context
		glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

		// Define the structure of a vertex for OpenGL to select values from vertex buffer
		// and store in vertexPos2DLocation attribute
		glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, x));

		// Enable the attribute to be passed vertices from the vertex buffer object
		glEnableVertexAttribArray(vertexUVLocation);
		// Define where the vertex specification will find the colour data and how much
		// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
		glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexNoNorm), (void*)offsetof(VertexNoNorm, u));

		glActiveTexture(GL_TEXTURE0);
		glUniform1i(samplerId, 0);
		glBindTexture(GL_TEXTURE_2D, b->getTexture()->getTextureName());

		// Draw the model
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Unselect the attribute from the context
		glDisableVertexAttribArray(vertexLocation);
		glDisableVertexAttribArray(vertexUVLocation);

		// Unselect the program from the context
		glUseProgram(0);

		//glDisable(GL_BLEND);
	}

}

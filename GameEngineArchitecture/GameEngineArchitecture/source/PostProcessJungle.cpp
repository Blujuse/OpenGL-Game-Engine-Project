#include "PostProcessJungle.h"
#include "FrameBuffer.h"
#include "ShaderUtils.h"
#include <iostream>

// Post process class has its own Vertex structure
// Doesn't have to and would recommend you use
// your existing Vertex structure.  PPVertex is
// so this class can be self contained.
struct PPVertex {
	// Location
	float x, y, z;
	float u, v;

	// Constructors
	// Sets vertex to passed values
	PPVertex(float _x, float _y, float _z, float _u, float _v) {
		// Location
		x = _x;
		y = _y;
		z = _z;

		// Colour
		u = _u;
		v = _v;
	}

	// Default, set to 0.0
	PPVertex() {
		x = y = z = 0.0f;
		u = v = 0.0f;
	}
};

// Define a quad into which texture will be
// post processed onto
static PPVertex quad[] = {
	PPVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
	PPVertex(-1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
	PPVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f),

	PPVertex(-1.0f, -1.0f, 0.0f, 0.0f, 0.0f),
	PPVertex(1.0f, -1.0f, 0.0f, 1.0f, 0.0f),
	PPVertex(1.0f, 1.0f, 0.0f, 1.0f, 1.0f)
};

bool GE::PostProcessJungle::init()
{
	// Create a framebuffer object for post processing
	// This is separate to other framebuffers
	// The idea is a texture is passed to this class for post
	// processing.
	glGenFramebuffers(1, &fboPostProcess);
	glBindFramebuffer(GL_FRAMEBUFFER, fboPostProcess);

	// Create a texture for the post process framebuffer.
	// Note we're not going to create a Texture object
	// because rendering to screen is self contained
	glGenTextures(1, &ppTextureName);
	glBindTexture(GL_TEXTURE_2D, ppTextureName);

	// Define the texture resolution and parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppTextureName, 0);

	// Note, there's no depth buffer because the post processing is done in 2D on the
	// texture of the scene.  Depth was taken into account when rendering the scene
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };

	// Set the list of draw buffers.
	glDrawBuffers(1, drawBuffers); // "1" is the size of DrawBuffers

	// Check post processing framebuffer has been created okay
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "FB error, status: " << Status << std::endl;
	}

	ppTexture = new Texture(ppTextureName);

	// Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create the vertex buffer for the quad
	glGenBuffers(1, &vboQuad);
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

	// Transfer vertices to graphics memory
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	// Create a program for the greyscale shaders
	std::string v_shader_source = loadShaderSourceCode("./assets/code/jungle.vert");
	std::string f_shader_source = loadShaderSourceCode("./assets/code/jungle.frag");

	// Due to the unique way OpenGL handles shader source, OpenGL expects
	// an array of strings.  In this case, create an array of the
	// loaded source code strings and pass to compileProgram for compilation
	const GLchar* v_source_array[] = { v_shader_source.c_str() };
	const GLchar* f_source_array[] = { f_shader_source.c_str() };

	// Compile shaders into a program
	if (!compileProgram(v_source_array, f_source_array, &programId)) {
		std::cerr << "Problem building texture renderer program.  Check console log for more information." << std::endl;
	}

	// Now get a link to the vertexPos3D so we can link the attribute
	// to our vertices when rendering
	vertexLocation = glGetAttribLocation(programId, "vertexPos");

	// Check for errors
	if (vertexLocation == -1) {
		std::cerr << "Problem getting vertex3D" << std::endl;
	}

	// Now link to the vUV attribute
	vertexUVLocation = glGetAttribLocation(programId, "vUV");

	// Check for errors
	if (vertexUVLocation == -1) {
		std::cerr << "Problem getting vUV" << std::endl;
	}

	// Uniforms
	samplerId = glGetUniformLocation(programId, "sampler");
	apply_effectLocation = glGetUniformLocation(programId, "apply_greyscale");

	return true;
}

void GE::PostProcessJungle::apply(Texture* tex)
{
	// This method renders the texture, tex, into a quad
	// It looks like other draw methods except there's no
	// camera as it's not necessary for 2D effect
	// Bind the post processing framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, fboPostProcess);

	// Define extents
	glViewport(0, 0, width, height);
	glDisable(GL_DEPTH_TEST);

	// Select the greyscale shaders program
	glUseProgram(programId);

	// Apply greyscale effect
	glUniform1i(apply_effectLocation, 1);

	// Select the vertex buffer object into the context
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

	// Enable the attribute to be passed vertices from the vertex buffer object
	glEnableVertexAttribArray(vertexLocation);
	// Define the structure of a vertex for OpenGL to select values from vertex buffer
	// and store in vertexPos2DLocation attribute
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(PPVertex), (void*)offsetof(PPVertex, x));

	// Enable the attribute to be passed vertices from the vertex buffer object
	glEnableVertexAttribArray(vertexUVLocation);
	// Define where the vertex specification will find the colour data and how much
	// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
	glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(PPVertex), (void*)offsetof(PPVertex, u));

	// Set the texture sampler to the texture wrapped
	// up in tex
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerId, 0);
	glBindTexture(GL_TEXTURE_2D, tex->getTextureName());

	// Draw the quad.  This will apply the greyscale shaders
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unselect the attribute from the context
	glDisableVertexAttribArray(vertexLocation);
	glDisableVertexAttribArray(vertexUVLocation);

	// Unselect the program from the context
	glUseProgram(0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
}

void GE::PostProcessJungle::renderToScreen()
{
	// This code presumes default framebuffer and
	// it has been cleared
	// Set the viewport for rendering to
	glViewport(0, 0, width, height);
	// Disable depth test as only 2D rendering
	// basically rendering a quad in ortho
	glDisable(GL_DEPTH_TEST);

	// Select the program into the rendering context
	glUseProgram(programId);

	// Don't apply greyscale effect as assuming
	// effect has already been applied and just
	// want to see the result on screen
	glUniform1i(apply_effectLocation, 0);

	// Select the vertex buffer object into the context
	glBindBuffer(GL_ARRAY_BUFFER, vboQuad);

	// Enable the attribute to be passed vertices from the vertex buffer object
	glEnableVertexAttribArray(vertexLocation);
	// Define the structure of a vertex for OpenGL to select values from vertex buffer
	// and store in vertexPos2DLocation attribute
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(PPVertex), (void*)offsetof(PPVertex, x));

	// Enable the attribute to be passed vertices from the vertex buffer object
	glEnableVertexAttribArray(vertexUVLocation);
	// Define where the vertex specification will find the colour data and how much
	// Colour data is four float values, located at where the r member is.  Stride is a vertex apart
	glVertexAttribPointer(vertexUVLocation, 2, GL_FLOAT, GL_FALSE, sizeof(PPVertex), (void*)offsetof(PPVertex, u));

	// Render the post processed texture, not the texture
	// before post processing
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(samplerId, 0);
	glBindTexture(GL_TEXTURE_2D, ppTextureName);

	// Draw the quad
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// Unselect the attribute from the context
	glDisableVertexAttribArray(vertexLocation);
	glDisableVertexAttribArray(vertexUVLocation);

	// Unselect the program from the context
	glUseProgram(0);

	glEnable(GL_DEPTH_TEST);
}
#include "FrameBuffer.h"
#include <iostream>

bool GE::FrameBuffer::init()
{
	// 2. Create a framebuffer object into which the current scene will be rendered
	// from the perspective of the light, not camera
	glGenFramebuffers(1, &fboScene);
	glBindFramebuffer(GL_FRAMEBUFFER, fboScene);

	// 3. Create the texture in which scene objects are rendered
	GLuint sceneTextureName;

	// Create a texture for the framebuffer.
	glGenTextures(1, &sceneTextureName);
	glBindTexture(GL_TEXTURE_2D, sceneTextureName);

	// 4. Define the texture format, size and parameters
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// 5. Bind texture to the depth component of the frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTextureName, 0);

	// 6. Create the depth buffer
	glGenRenderbuffers(1, &depthBufferName);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBufferName);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferName);

	// 7. Set the list of attachments (draw buffers) the framebuffer has
	// has one by default where rasterised scene goes
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers); // "1" is the size of DrawBuffers

	// 8. Was the framebuffer created okay
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << " FB error, status: " << Status << std::endl;

		return false;
	}

	// 9. Unbind framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// 10. Create a Texture object so we can apply it to a model later
	sceneTexture = new Texture(sceneTextureName);

	return true;
}
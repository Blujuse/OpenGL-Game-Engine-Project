#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "Texture.h"

namespace GE {
	class FrameBuffer
	{
	public:
		FrameBuffer(int w, int h) {
			width = w;
			height = h;

			fboScene = -1;
			sceneTexture = nullptr;
			depthBufferName = -1;
		}

		~FrameBuffer() {
			// Release the texture, depth buffer and framebuffer objects
			delete sceneTexture;
			glDeleteRenderbuffers(1, &depthBufferName);
			glDeleteFramebuffers(1, &fboScene);
		}

		bool init();

		void beginRender() {
			// 11. Bind the framebuffer to the pipeline
			// All objects rendered from this point go to the
			// framebuffer until glBindBuffer is used with a different framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, fboScene);
		}

		void endRender() {
			// 12. Bind the default framebuffer.  No more objects
			// will be rendered to the framebuffer encapsulated in this class
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		// Provides access to the texture object
		// associated with the framebuffer
		Texture* getTexture() {
			return sceneTexture;
		}

	private:
		// 1. Member variables
		GLuint fboScene; // Framebuffer object
		GLuint depthBufferName; // Texture name

		Texture* sceneTexture; // Texture into which scene is rendered

		int width, height;
	};

}
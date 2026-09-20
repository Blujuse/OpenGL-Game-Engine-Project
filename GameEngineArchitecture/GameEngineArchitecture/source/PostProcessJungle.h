#pragma once
#include <GL/glew.h>
#include <SDL_opengl.h>
#include "Texture.h"

namespace GE 
{
	class PostProcessJungle
	{
	public:
		PostProcessJungle(int w, int h) {
			width = w;
			height = h;

			fboPostProcess = -1;
			ppTexture = nullptr;
		}

		~PostProcessJungle() {
			// Release the texture, depth buffer and framebuffer objects
			delete ppTexture;
			glDeleteFramebuffers(1, &fboPostProcess);
		}

		bool init();

		void apply(Texture* tex);

		void renderToScreen();

		// Provides access to the texture object
		// associated with the framebuffer
		Texture* getTexture() {
			return ppTexture;
		}

	private:
		// Member variables
		// Framebuffer variables like before
		GLuint fboPostProcess;

		// Texture into which post processed
		// texture is rendered
		GLuint ppTextureName;

		// Need a quad to render itno
		GLuint vboQuad;

		// And a shader program to render
		// a texture in greyscale
		GLuint programId;
		GLuint vertexLocation;
		GLuint vertexUVLocation;
		GLuint apply_effectLocation;
		GLuint samplerId;

		// Wraps the texture object so it
		// can be used by a scene object or
		// passed to another post processor
		Texture* ppTexture;

		int width, height;
	};

}
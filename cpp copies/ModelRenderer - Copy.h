#pragma once

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_opengl.h>
#include <glm.hpp>
#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Terrain.h"
#include "Skydome.h"

namespace GE 
{
	class ModelRenderer
	{

	public:
		ModelRenderer();

		virtual ~ModelRenderer();

		void init();

		void Update();

		void Draw(Camera* cam, Model *model);

		void DrawTerrain(Camera* cam, Terrain* terrain);

		void DrawSkydome(Camera* cam, Skydome* skydome);
		
		void Destroy();

		void DisplayShaderCompileError(GLuint shaderId);

#pragma region Gets & Sets

		float getPosX();
		float getPosY();
		float getPosZ();
		float getRotX();
		float getRotY();
		float getRotZ();
		float getScaleX();
		float getScaleY();
		float getScaleZ();

		// Mutator methods
		void setPos(float x, float y, float z);
		void setRot(float rX, float rY, float rZ);
		void setScale(float sX, float sY, float sZ);

		void setTexture(Texture* _tex);

#pragma endregion

	private:

		// This stores the program object that contains the shaders
		GLuint programId;

		// This stores the attribute to select into the pipeline
		// to link the triangle vertices to the pipeline
		GLint vertexPos3DLocation;

		// Link to vColour attribute which recieves the colour in a vertex
		GLint vertexUVLocation;

		// Link to normal stuff
		GLint vertexNormal;

		// This stores the triangle vertex buffer object containing the vertices
		// transferred from this code to the graphics memory
		GLuint vboTriangle;

		// Location, rotation and scale variables
		float posX, posY, posZ;
		float rotX, rotY, rotZ;
		float scaleX, scaleY, scaleZ;

		Texture* tex;

		// GLSL uniform variables for the transformation, view and projection matrices
		GLuint transformUniformId;
		GLuint viewUniformId;
		GLuint projectionUniformId;
		GLuint samplerId;
		// Cam pos
		GLuint viewPosId;

		// Fog uniform link variables
		GLuint fogColourId;
		GLuint fogDensityId;
	};
}
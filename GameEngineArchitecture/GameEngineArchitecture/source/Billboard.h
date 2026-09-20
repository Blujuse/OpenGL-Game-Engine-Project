#pragma once
#include "Texture.h"

namespace GE
{
	class Billboard
	{
	public:
		// Constructor
		Billboard(Texture* texture, float posX, float posY, float posZ)
		{
			billTex = texture;

			x = posX;
			y = posY;
			z = posZ;

			scaleX = scaleY = scaleZ = 1.0f;
		}

		// Nothing going on in destructor
		~Billboard() 
		{

		}

		float getX() 
		{ 
			return x; 
		}

		float getY()
		{
			return y;
		}

		float getZ()
		{
			return z;
		}

		void setX(float newX)
		{
			x = newX;
		}

		void setY(float newY)
		{
			y = newY;
		}

		void setZ(float newZ)
		{
			z = newZ;
		}

		// Return the texture
		Texture* getTexture()
		{
			return billTex;
		}

		void setScaleX(float newX)
		{
			scaleX = newX;
		}

		void setScaleY(float newY)
		{
			scaleY = newY;
		}

		float getScaleX()
		{
			return scaleX;
		}

		float getScaleY()
		{
			return scaleY;
		}

	private:
		// Data
		// Position
		float x, y, z;

		// Scale
		float scaleX, scaleY, scaleZ;

		// Texture
		Texture* billTex;
	};
}
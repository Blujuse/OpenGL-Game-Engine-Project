#include "Terrain.h"
#include "Model.h"
#include "Vertex.h"
#include <SDL_image.h>
#include <vector>
#include <string>

namespace GE 
{
	Terrain::Terrain(std::string heightMapFilename, float passthroughHeight, float passthroughScale)
	{
		// 1. Load the heightmap image from a file.  It would be better to
		// pass the name of the heightmap image file to the constructor rather
		// than hard code the name into the constructor
		SDL_Surface* heightMap = IMG_Load(heightMapFilename.c_str());

		// 2. Get the resolution of the heightmap image as it defines the
		// vertices in the terrain.  Each pixel in image corresponds to a
		// vertex
		int height = heightMap->h;
		int width = heightMap->w;

		// 3. Create a pointer variable as a means of accessing the pixels
		// in the image.  Pixel colour defines the Y of a vertex
		unsigned char* imageData = (unsigned char*)heightMap->pixels;

		// 4. Get the number of channels as need this to work out
		// where red colour is in a specific pixel.
		int channels = heightMap->format->BytesPerPixel;

		// 5. Define height and size scales of the terrain.  Again, it would
		// be better to pass these values as parameters to constructor instead
		// of hard code them in the constructor
		float heightScale = passthroughHeight;
		float scale = passthroughScale;

		// 6. Vectors for storing the vertices and indices (of vertices)
		std::vector<VertexNoNorm> vertices;
		std::vector<GLuint> indices;

		// 7. These loops create the vertices for the terrain
		// Size of terrain is based on heightmap resolution width and height
		float x, y, z, u, v;

		for (int row = 0; row < height; row++)
		{
			// For each vertex in a row
			for (int column = 0; column < width; column++)
			{
				// Find the red pixel for the vertex.  (row * width) + column works out
				// the vertex and * channels finds the byte corresponding to red for the
				// vertex.  A pixel may be made up of more than one byte for example
				// each channel could a byte so a pixel colour would be three bytes
				unsigned char redPixel = imageData[((row * width) + column) * channels];

				// Calculate the vertex x, y, z based on column and row
				// (x and z value), y is determined from the amount of red
				// no red means height would be 0, all red means height would
				// be maximum
				// Note x,y,z are scaled valuess
				x = ((float)column / (float)(width - 1) - 0.5f) * scale;
				z = ((float)row / (float)(height - 1) - 0.5f) * scale;
				y = (redPixel / 255.0f) * heightScale;

				// Calculate the uv based where the vertex is within the 
				// terrain in terms of its column and row
				// Remember uv values are in the range of 0 to 1
				u = (float)column / (float)(width - 1);
				v = (float)(height - row) / (float)(height - 1);

				// Add the vertex to the vector.  Vector will be used
				// to create the vertex buffer
				vertices.push_back(VertexNoNorm(x, y, z, u, v, 0.0f, 0.0f, 0.0f));
			}				
		}

		// 8. These loops create the indices for the index buffer
		// See slides 16 and 17 in the lecture notes
		unsigned int c, n;
		for (int row = 0; row < height - 1; row++)
		{
			for (int column = 0; column < width - 1; column++)
			{
				c = row * width + column;
				n = (row + 1) * width + column;

				indices.push_back(c);
				indices.push_back(n);
				indices.push_back(c + 1);
				indices.push_back(c + 1);
				indices.push_back(n);
				indices.push_back(n + 1);
			}
		}

		// 9 .Create the vertex buffer.  Same as creating vertex buffer for model
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexNoNorm), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 10. Create the index buffer.  Same way as creating a vertex buffer except
		// type is GL_ELEMENT_ARRAY_BUFFER and use the indices vector
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// 11. Store the number of indices as need this for rendering
		// the terrain
		indexCount = indices.size();

		// 12. Tidy up
		SDL_FreeSurface(heightMap);
	}
}
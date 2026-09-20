#include "Skydome.h"
#include "Vertex.h"
#include <vector>
#include <cmath>

namespace GE 
{
	Skydome::Skydome() 
	{
		// 1. Define the dome in terms of resolution
		// and size (radius).  Note, some of these
		// constants would be better as parameters
		// to the skydome constructor to configure
		// skydome at runtime, for example

		// Used to determine extent of dome in terms
		// of 90 degrees (radians)
		const float PIDIV2 = M_PI / 2.0f;

		// Define extents of skydome in terms of numbers
		// of vertices horizontally and vertically
		int horizResolution = 16;
		int vertResolution = 16;

		// Radius for sphere
		int radius = 10;

		// Store the generated vertices
		std::vector<VertexNoNorm> vertices;

		// Angle between each horizontal vertex in radians
		// Angles in radians are expressed in terms of PI
		// This code makes use of math.h 
		float azimuthStep = 2.0f * M_PI / horizResolution;

		// Texture extent.  This would be changed depending
		// on the skydome texture.  For example, some textures
		// are meant to be wrapped on a sphere, not a dome.
		// If you use a sphere percentage of 1.0, then the 
		// bottom part of the texture appears at the based 
		// of the dome.  In this case, change texturePercentage
		// to 0.5f so v ranges from 0 to 0.5 and only
		// top half of texture is shown
		float texturePercentage = 1.0f;

		// Determines the extent of the dome in terms of a
		// percentage of PIDIV2.  1.0 means 100% of PIDIV2
		// or 90 degrees.  2.0 means 200% of PIDIV2 which
		// results in a sphere
		float spherePercentage = 1.0f;

		// Starting point for vertices.  Vertices start from top
		float elevation = PIDIV2;

		// Calculate the vertical step between vertices
		// based 90 degree * sphere percentage
		float elevationStep = spherePercentage * PIDIV2 / vertResolution;

		// Steps horizontally and vertically across uv range 0 to 1
		float uStep = 1.0f / horizResolution;
		float vStep = 1.0f / vertResolution;

		// Starting point for v texture co-ord.  Bottom of texture
		float v = 0.0f;

		// 2. Determine the vertices horizontally for each elevation
		// from the top to bottom.
		for (int vert = 0; vert < vertResolution + 1; vert++)
		{
			// Calculate horizontal length for elevation, 0 at top to radius at 0 degrees
			float length = radius * cos(elevation);

			// Calculate y position
			float y = radius * sin(elevation);

			float u = 0.0f;
			float angle = 0;

			// Calculate horizontal vertices for elevation
			for (int horiz = 0; horiz < horizResolution + 1; horiz++)
			{
				// Calculate x and z positions
				float x = length * cos(angle);
				float z = length * sin(angle);

				// Add the vertex to the vector
				vertices.push_back(VertexNoNorm(x, y, z, u, v * texturePercentage, 0.0f, 0.0f, 0.0f));

				// Move to next horizontal vertex
				angle += azimuthStep;

				u += uStep;
			}

			elevation -= elevationStep;
			v += vStep;
		}

		// 3. Determine the indices
		std::vector<int> indices;

		// This code generates indices in quads, as two triangles,
		// from top of dome down.  Quads at the top and bottom
		// appear as triangles
		// 
		//	(offset + horiz + 1)                           ._   . (offset + horiz)
		//                                                 | // |
		//  (offset + (horizResolution + 1) + horiz + 1)   .   -. (offset + horizResolution + (horiz + 1))
		for (int vert = 0; vert < vertResolution; vert++)
		{
			int offset = vert * (horizResolution + 1);

			for (int horiz = 0; horiz < horizResolution; horiz++)
			{
				// Left triangle of quad
				indices.push_back(offset + horiz);
				indices.push_back(offset + horiz + 1);
				indices.push_back(offset + (horizResolution + 1) + (horiz + 1));

				// Right triangle of quad
				indices.push_back(offset + (horizResolution + 1) + (horiz + 1));
				indices.push_back(offset + horizResolution + (horiz + 1));
				indices.push_back(offset + horiz);
			}
		}

		// 4 .Create the vertex buffer.  Same as creating vertex buffer for model
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexNoNorm), vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 5. Create the index buffer.  Same way as creating a vertex buffer except
		// type is GL_ELEMENT_ARRAY_BUFFER and use the indices vector
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// 6. Store the number of indices as need this for rendering
		// the terrain
		indexCount = indices.size();
	}

	Skydome::~Skydome()
	{
		// Delete the vertex buffer
		glDeleteBuffers(1, &vbo);

		// Delete the index buffer
		glDeleteBuffers(1, &ibo);
	}
}
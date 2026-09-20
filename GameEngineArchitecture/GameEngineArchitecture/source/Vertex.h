#pragma once

namespace GE
{
	// Struct to represent a vertex in the engine
	struct Vertex
	{
		// Location
		float x, y, z;
		float u, v;
		float nx, ny, nz;
		float tx, ty, tz;
		float btx, bty, btz;

		// Constructors
		// Sets vertex to passed values
		Vertex(float vX, float vY, float vZ, float vU, float vV, float nX, float nY, float nZ, 
			float _tx, float _ty, float _tz, float _btx, float _bty, float _btz)
		{
			// Location
			x = vX;
			y = vY;
			z = vZ;

			// Colour
			u = vU;
			v = vV;

			// Normals
			nx = nX;
			ny = nY;
			nz = nZ;

			// Tangents
			tx = _tx;
			ty = _ty;
			tz = _tz;

			// Bitangents
			btx = _btx;
			bty = _bty;
			btz = _btz;
		}

		// Sets vertex to origin
		Vertex()
		{
			x = y = z = 0.0f;
			u = v = 0.0f;
			nx = ny = nz = 0.0f;
		}
	};

	struct VertexNoNorm
	{
		// Location
		float x, y, z;
		float u, v;
		float nx, ny, nz;

		// Constructors
		// Sets vertex to passed values
		VertexNoNorm(float vX, float vY, float vZ, float vU, float vV, float nX, float nY, float nZ)
		{
			// Location
			x = vX;
			y = vY;
			z = vZ;

			// Colour
			u = vU;
			v = vV;

			// Normals
			nx = nX;
			ny = nY;
			nz = nZ;
		}

		// Sets vertex to origin
		VertexNoNorm()
		{
			x = y = z = 0.0f;
			u = v = 0.0f;
			nx = ny = nz = 0.0f;
		}
	};
}
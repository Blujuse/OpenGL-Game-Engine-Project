#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Model.h"
#include "Vertex.h"

namespace GE
{
	enum ModelType
	{
		NORMAL = 0,
		NONORMAL = 1
	};

	bool Model::LocalLoadFromFile(const char* filename, int usingNormals)
	{
		// Temporary storage for the vertices
		std::vector<Vertex> loadedVertices;
		std::vector<VertexNoNorm> loadedVerticesNoNorm;

		// Create importer object
		Assimp::Importer importer;

		// Load model into scene object
		const aiScene* pScene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

		// Check if file was opened
		if (!pScene)
		{
			return false;
		}

		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++)
		{
			// Get the mesh
			const aiMesh* mesh = pScene->mMeshes[MeshIdx];

			// Loop through the vertices
			for (int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++)
			{
				// Get the face
				const aiFace& face = mesh->mFaces[faceIdx];

				// Extract a vertex from the mesh's main vertex array
				// for each point in the face, 3 as there are three vertices in a face (triangulated)
				for (int vertIdx = 0; vertIdx < face.mNumIndices; vertIdx++)
				{
					// Extract pos and tex coords based on the index number, not direct from mesh arrays
					const aiVector3D* pos = &mesh->mVertices[face.mIndices[vertIdx]];

					// Get uvs for vertex, thus code assumes there are uvs
					// defined in the model vertices. if not, then code will crash
					const aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[vertIdx]];

					// Face normals
					const aiVector3D* norm = &mesh->mNormals[face.mIndices[vertIdx]];

					if (usingNormals == NORMAL) 
					{
						// Tangents
						const aiVector3D* tang = &mesh->mTangents[face.mIndices[vertIdx]];

						// Bitangents
						const aiVector3D* bitang = &mesh->mBitangents[face.mIndices[vertIdx]];

						// Create new object in the shape array based on the extracted vertex
						// This shape array will be used to create the vertex buffer
						loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y, norm->x, norm->y, norm->z,
							tang->x, tang->y, tang->z, bitang->x, bitang->y, bitang->z));
					}
					else
					{
						// Create new object in the shape array based on the extracted vertex
						// This shape array will be used to create the vertex buffer
						loadedVerticesNoNorm.push_back(VertexNoNorm(pos->x, pos->y, pos->z, uv.x, uv.y, norm->x, norm->y, norm->z));
					}
				}
			}
		}

		if (usingNormals == NORMAL)
		{
			// Number of vertices is derived from the number of items in the temp vector
			numVertices = loadedVertices.size();
		}
		else
		{
			numVertices = loadedVerticesNoNorm.size();
		}

		// Copy vertices into a memory buffer in order to transfer to a vbo later
		glGenBuffers(1, &vbo);

		// Create the buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		if (usingNormals == NORMAL)
		{
			// Copy vertex data from vector to buffer
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), loadedVertices.data(), GL_STATIC_DRAW);
		}
		else
		{
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(VertexNoNorm), loadedVerticesNoNorm.data(), GL_STATIC_DRAW);
		}

		// Remove the buffer from the pipeline
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// Must be good if got this far
		return true;
	}

	bool Model::LoadFromFile(const char* filename)
	{
		return LocalLoadFromFile(filename, NORMAL);
	}

	bool Model::LoadFromFileNoNorm(const char* filename)
	{
		return LocalLoadFromFile(filename, NONORMAL);
	}
}
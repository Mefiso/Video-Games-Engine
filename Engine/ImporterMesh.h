#pragma once
#include "Globals.h"
#include "CMesh.h"
#include <assimp/mesh.h>
#include <GL/glew.h>
#include <Math/float3.h>
#include <Math/float2.h>

class GameObject;
namespace ImporterMesh
{
	bool			Import(const aiMesh* _aimesh, GameObject* _owner, std::string& _path);		// Imports a Mesh from Assimp into a GameObject '_parent'
	bool			Load(const char* _filename, GameObject* _owner, unsigned int _filesize);	// Loads a Mesh from a file in custom file format
	unsigned int	Save(CMesh* _mesh, const char* _filename);									// Saves the information of a Component Mesh into a file, using our custom file format

	namespace // Private functions in the namespace
	{
		// If vertex is a point outside the current min and max points for AABB, it modifies those min/max points
		void CheckMinMaxPoints(float* _vertex, CMesh* _cmesh)
		{
			float3 AABBmax = _cmesh->GetAABBmax();
			float3 AABBmin = _cmesh->GetAABBmin();

			AABBmax.x = _vertex[0] > AABBmax.x ? _vertex[0] : AABBmax.x;
			AABBmax.y = _vertex[1] > AABBmax.y ? _vertex[1] : AABBmax.y;
			AABBmax.z = _vertex[2] > AABBmax.z ? _vertex[2] : AABBmax.z;

			AABBmin.x = _vertex[0] < AABBmin.x ? _vertex[0] : AABBmin.x;
			AABBmin.y = _vertex[1] < AABBmin.y ? _vertex[1] : AABBmin.y;
			AABBmin.z = _vertex[2] < AABBmin.z ? _vertex[2] : AABBmin.z;

			_cmesh->SetAABBmax(AABBmax);
			_cmesh->SetAABBmin(AABBmin);
		}
		// ------------------------------------------------------------------------------------- //

		// Loads the Vertex Buffer Object of this mesh
		unsigned int LoadVBO(CMesh* _cmesh, unsigned int _numVertices, float* _verts, unsigned int _vtxSize)
		{
			unsigned int VBO = 0u;
			unsigned buff_size = sizeof(float) * _vtxSize * _numVertices;
			glGenBuffers(1, &VBO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, buff_size, _verts, GL_STATIC_DRAW);
			return VBO;
		}
		// ------------------------------------------------------------------------------------- //

		// Loads the Element Buffer Object of this mesh
		unsigned int LoadEBO(unsigned int _numIndices, unsigned int* _indices)
		{
			unsigned int EBO = 0u;
			unsigned index_size = sizeof(unsigned) * _numIndices;
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, _indices, GL_STATIC_DRAW);
			return EBO;
		}
		// ------------------------------------------------------------------------------------- //

		// Loads the Vertex Array Object of this mesh
		unsigned int CreateVAO(unsigned int _VBO, unsigned int _EBO)
		{
			unsigned int VAO = 0u;
			glGenVertexArrays(1, &VAO);

			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);

			// TODO: what if normals or uvs are not present?
			// vertex positions
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
			// vertex normals
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 3));
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(sizeof(float) * 6));

			glBindVertexArray(0);
			return VAO;
		}
		// ------------------------------------------------------------------------------------- //
	}
};

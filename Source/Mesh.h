#pragma once
#include <vector>
#include "MATH_GEO_LIB/Math/float3.h"
#include "ASSIMP/mesh.h"

class Mesh
{
private:
	float* vertices; // position_1.x, position_1.y, positio_1.z, normal_1.x, normal_1.y, normal_1.z, texture_1.x, texture_1.y, ...
	unsigned int* indices; // first_index_1, second_index_1, third_index_1, ...

	unsigned int vertex_array_object;
	unsigned int vertex_buffer_object;
	unsigned int element_buffer_object;	

	size_t number_of_vertices;
	size_t number_of_indices;

public:
	Mesh();
	~Mesh();

	void Load(const aiMesh* mesh_data);
	void Draw(const unsigned int* model_textures) const;

private:
	void LoadMeshData(const aiMesh* mesh_data);
};
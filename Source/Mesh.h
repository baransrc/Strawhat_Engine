#pragma once
#include <vector>
#include "MATH_GEO_LIB/Math/float3.h"
#include "ASSIMP/mesh.h"
#include "MATH_GEO_LIB/Geometry/AABB.h"

class Mesh
{
private:
	float* vertices; // position_1.x, position_1.y, positio_1.z, normal_1.x, normal_1.y, normal_1.z, texture_1.x, texture_1.y, ...
	unsigned int* indices; // first_index_1, second_index_1, third_index_1, ...

	unsigned int vertex_array_object;
	unsigned int vertex_buffer_object;
	unsigned int element_buffer_object;	
	
	math::AABB bounding_box;

	size_t number_of_vertices;
	size_t number_of_indices;
	size_t number_of_triangles;

public:
	Mesh();
	~Mesh();

	void Load(const aiMesh* mesh_data);
	void Draw(const unsigned int* model_textures) const;

	size_t GetNumberOfVertices() const { return number_of_vertices; };
	size_t GetNumberOfIndices() const { return number_of_indices; };
	size_t GetNumberOfTriangles() const { return number_of_triangles; };
	const math::AABB* GetAABB() const { return &bounding_box; };

private:
	void LoadMeshData(const aiMesh* mesh_data);
	void LoadAABB(const aiMesh* mesh_data);
};
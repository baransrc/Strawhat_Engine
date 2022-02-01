#pragma once

#include "Component.h"

#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Geometry/AABB.h"

#include <vector>

class ComponentMesh : public Component
{
private:
	float* vertices; // position_1.x, position_1.y, position_1.z, normal_1.x, normal_1.y, normal_1.z, texture_1.x, texture_1.y, ...
	unsigned int* indices; // first_index_1, second_index_1, third_index_1, ...
	unsigned int* texture_ids;

	unsigned int vertex_array_object;
	unsigned int vertex_buffer_object;
	unsigned int element_buffer_object;

	math::AABB bounding_box;

	size_t number_of_vertices;
	size_t number_of_indices;
	size_t number_of_triangles;
	size_t number_of_texture_ids;

	bool is_currently_loaded;
	bool is_culled;

public:
	ComponentMesh();
	~ComponentMesh() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load(
		float* new_vertices, 
		unsigned int* new_indices, 
		const unsigned int* new_texture_ids, 
		size_t new_number_of_vertices, 
		size_t new_number_of_indices, 
		size_t new_number_of_triangles,
		size_t new_number_of_texture_ids
	);
	void Update() override;
	void Reset();
	void DrawGizmo() override;

	size_t GetNumberOfVertices() const { return number_of_vertices; };
	size_t GetNumberOfIndices() const { return number_of_indices; };
	size_t GetNumberOfTriangles() const { return number_of_triangles; };
	const math::AABB& GetAABB() const { return bounding_box; };

	void SetCulled(bool new_is_culled) const;
	bool IsCulled() const;


protected:
	void DrawInspectorContent() override;

private:
	void LoadAABB();
};


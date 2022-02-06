#pragma once

#include "Component.h"

#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Geometry/AABB.h"

#include <vector>

class ComponentMesh : public Component
{
private:
	/// <summary>
	/// Vertices of this ComponentMesh. Vertex data is stored as an 
	/// interleaved array as follows: 
	/// position_1.x, position_1.y, position_1.z, normal_1.x, normal_1.y, normal_1.z, texture_1.x, texture_1.y, ...
	/// </summary>
	float* vertices; 

	/// <summary>
	/// Indices of this ComponentMesh. Indices data is stored as follows:
	/// first_index_1, second_index_1, third_index_1, ...
	/// </summary>
	unsigned int* indices;

	/// <summary>
	/// VAO ID of this ComponentMesh.
	/// </summary>
	unsigned int vertex_array_object;

	/// <summary>
	/// VBO ID of this ComponentMesh.
	/// </summary>
	unsigned int vertex_buffer_object;

	/// <summary>
	/// EBO ID of this ComponentMesh.
	/// </summary>
	unsigned int element_buffer_object;

	/// <summary>
	/// AABB that encloses the vertices of this ComponentMesh.
	/// </summary>
	math::AABB bounding_box;

	/// <summary>
	/// Number of vertices this ComponentMesh has.
	/// </summary>
	size_t number_of_vertices;

	/// <summary>
	/// Number of indices this ComponentMesh has.
	/// </summary>
	size_t number_of_indices;
	
	/// <summary>
	/// Number of triangles this ComponentMesh has.
	/// This should be equal to number_of_indices / 3,
	/// but stored anyway.
	/// </summary>
	size_t number_of_triangles;

	/// <summary>
	/// This flag is set to true if Initialize or constructor is called, 
	/// false if Reset or destructor is called.
	/// </summary>
	bool is_currently_loaded;

	/// <summary>
	/// This flag is set to true by the scene when this mesh is outside
	/// the render camera's frustum, false otherwise.
	/// </summary>
	bool is_culled;

	/// <summary>
	/// This vector stores triangles of the mesh for convenience for
	/// the intersection functions, mouse picking etc.
	/// This is a special std::vector<math::Triangle> under the hood.
	/// </summary>
	math::TriangleArray cached_triangles;

public:
	ComponentMesh();
	~ComponentMesh() override;

	/// <returns> 
	/// Type of this Component, MESH.
	/// </returns>
	component_type Type() const override;
	
	/// <summary>
	/// Initializes this ComponentMesh along with the provided owner.
	/// </summary>
	/// <param name="new_owner">Entity to be set as the owner.</param>
	void Initialize(Entity* new_owner) override;
	
	/// <summary>
	/// Loads this mesh with provided vertices and indices.
	/// Also sets is_currently_loaded to true.
	/// </summary>
	/// <param name="new_vertices">Vertices array to be copied into vertices.</param>
	/// <param name="new_indices">Indices array to be copied into indices.</param>
	/// <param name="new_number_of_vertices">Value to be set as number of vertices.</param>
	/// <param name="new_number_of_indices">Value to be set as number of indices.</param>
	/// <param name="new_number_of_triangles">Value to be set as number of triangles.</param>
	void Load(
		float* new_vertices, 
		unsigned int* new_indices, 
		size_t new_number_of_vertices, 
		size_t new_number_of_indices, 
		size_t new_number_of_triangles
	);
	
	/// <summary>
	/// Called every Update of owner Entity.
	/// </summary>
	void Update() override;
	
	/// <summary>
	/// Resets this ComponentMesh like it has never been Initialized.
	/// Also sets is_currently_loaded to false.
	/// </summary>
	void Reset();
	
	/// <summary>
	/// Called on DrawGizmos of owner Entity.
	/// </summary>
	void DrawGizmo() override;

	/// <returns> 
	/// Number of vertices this ComponentMesh has.
	/// </returns>
	size_t GetNumberOfVertices() const { return number_of_vertices; };
	
	/// <returns> 
	/// Number of indices this ComponentMesh has.
	/// </returns>
	size_t GetNumberOfIndices() const { return number_of_indices; };
	
	/// <returns> 
	/// Number of triangles this ComponentMesh has.
	/// </returns>
	size_t GetNumberOfTriangles() const { return number_of_triangles; };
	
	/// <returns> 
	/// Local AABB that encloses the vertices of this ComponentMesh.
	/// </returns>
	const math::AABB& GetAABB() const { return bounding_box; };
	
	/// <returns> 
	/// Vertices of this ComponentMesh.
	/// </returns>
	const float* GetVertices() const { return vertices; };
	
	/// <returns> 
	/// Triangles of this ComponentMesh.
	/// </returns>
	const math::TriangleArray& GetTriangles() const { return cached_triangles; }

	/// <summary>
	/// Sets is_culled flag of this ComponentMesh.
	/// </summary>
	/// <param name="new_is_culled">True if this ComponentMesh is meant to be culled, false otherwise.</param>
	void SetCulled(bool new_is_culled);
	
	/// <returns> 
	/// True if this ComponentMesh is culled by the main camera of the scene, false otherwise.
	/// </returns>
	bool IsCulled() const;

protected:
	/// <summary>
	/// Called by Component::DrawInspector.
	/// </summary>
	void DrawInspectorContent() override;

private:
	/// <summary>
	/// Loads the AABB that encloses this ComponentMesh by traversing
	/// its vertices array. This is a costly function, use this as less
	/// as possible.
	/// </summary>
	void LoadAABB();
};


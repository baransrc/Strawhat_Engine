#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "Entity.h"
#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "GLEW/include/GL/glew.h"
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"
#include "MATH_GEO_LIB/Geometry/Triangle.h"


ComponentMesh::ComponentMesh() : 
	Component(),
	vertices(nullptr),
	indices(nullptr),
	vertex_array_object(0),
	vertex_buffer_object(0),
	element_buffer_object(0),
	bounding_box(),
	number_of_vertices(0),
	number_of_indices(0),
	number_of_triangles(0),
	is_currently_loaded(false),
	is_culled(false)

{
}

ComponentMesh::~ComponentMesh()
{
	Reset();
}

component_type ComponentMesh::Type() const
{
	return component_type::MESH;
}

void ComponentMesh::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);
}

void ComponentMesh::Load(float* new_vertices, unsigned int* new_indices, size_t new_number_of_vertices, size_t new_number_of_indices, size_t new_number_of_triangles)
{
	// If Load was called before this call, clear 
	// all the previous mesh data and load afterwards:
	if (is_currently_loaded)
	{
		Reset();
	}

	// Get vertices:
	vertices = new_vertices;
	// Get indices:
	indices = new_indices;

	// Get supplied number of vertices, indices and triangles:
	number_of_vertices = new_number_of_vertices;
	number_of_indices = new_number_of_indices;
	number_of_triangles = new_number_of_triangles;

	// Cache triangles for easy access:
	cached_triangles.reserve(number_of_triangles);

	for (size_t i = 0; i < number_of_indices; i += 3)
	{
		size_t index_1 = indices[i];
		size_t index_2 = indices[i + 1];
		size_t index_3 = indices[i + 2];

		math::float3 a(vertices[index_1 * 8], vertices[index_1 * 8 + 1], vertices[index_1 * 8 + 2]);
		math::float3 b(vertices[index_2 * 8], vertices[index_2 * 8 + 1], vertices[index_2 * 8 + 2]);
		math::float3 c(vertices[index_3 * 8], vertices[index_3 * 8 + 1], vertices[index_3 * 8 + 2]);
		
		cached_triangles.push_back(math::Triangle(a, b, c));
	}

	// Load AABB:
	LoadAABB();

	// Generate VAO:
	glGenVertexArrays(1, &vertex_array_object);
	// Generate VBO:
	glGenBuffers(1, &vertex_buffer_object);
	// Generate EBO:
	glGenBuffers(1, &element_buffer_object);

	// Bind VAO:
	glBindVertexArray(vertex_array_object);

	// Bind VBO:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	// Allocate memory and store data within the initialized 
	// memory in the currently bound vertex buffer object 
	// a.k.a VBO with id vertex_buffer_object:
	glBufferData(GL_ARRAY_BUFFER, number_of_vertices * 8 * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	// Bind EBO:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	// Allocate memory and store data within the initialized 
	// memory in the currently bound vertex buffer object 
	// a.k.a EBO with id element_buffer_object:
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, number_of_indices * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	size_t vertices_size = sizeof(float) * 8;

	// Enable position attribute:
	glEnableVertexAttribArray(0);
	// Give position/size/data-type/stride of position attributes:
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertices_size, (void*)0); // Offset by 0 floats

	// Enable normal attribute:
	glEnableVertexAttribArray(1);
	// Give position/size/data-type/stride of normal attributes:
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vertices_size, (void*)(sizeof(float) * 3)); // Offset by 3 floats (position)

	// Enable texture-coordinates attribute:
	glEnableVertexAttribArray(2);
	// Give position/size/data-type/stride of texture-coordinates attributes:
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vertices_size, (void*)(sizeof(float) * 6)); // Offset by 6 floats (position + normal)

	// Unbind VAO with id vertex_array_object_id:
	glBindVertexArray(0);

	// Set as currently loaded:
	is_currently_loaded = true;

	// Invoke change in parent and its ancestors:
	owner->InvokeComponentsChangedEvents(Type());
}

void ComponentMesh::Update()
{
	if (!Enabled())
	{
		return;
	}

	if (is_culled)
	{
		return;
	}

	ComponentMaterial* material = (ComponentMaterial*) owner->GetComponent(component_type::MATERIAL);

	if (material != nullptr)
	{
		App->shader_program->Use();
		App->shader_program->SetUniformVariable("model_matrix", owner->Transform()->GetMatrix(), true);

		material->Use();
	}
	else 
	{
		// Use the shader:
		App->shader_program->Use();
		App->shader_program->SetUniformVariable("model_matrix", owner->Transform()->GetMatrix(), true);
	}

	// Bind VAO:
	glBindVertexArray(vertex_array_object);
	// Draw Mesh with VBO and EBO:
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, nullptr);
	// Unbind VAO:
	glBindVertexArray(0);
}

void ComponentMesh::Reset()
{
	free(indices);
	free(vertices);

	indices = nullptr;
	vertices = nullptr;

	if (is_currently_loaded)
	{
		glDeleteBuffers(1, &element_buffer_object);
		glDeleteBuffers(1, &vertex_buffer_object);
		glDeleteVertexArrays(1, &vertex_array_object);
	}

	is_currently_loaded = false;
}

void ComponentMesh::DrawGizmo()
{
	// NOTE: If you add a gizmo for ComponentMesh,
	// check for the following:
	if (!Enabled() || !owner->IsActive())
	{
		return;
	}

	// Uncomment this if you want to debug the 
	// triangles, but beware, this f**ks up the 
	// framerate:
	/*
		for (const math::Triangle& triangle : cached_triangles)
		{
			App->debug_draw->DrawTriangle(triangle, math::float3(1.0f, 1.0f, 0.0f));
		}
	*/
}

void ComponentMesh::SetCulled(bool new_is_culled)
{
	is_culled = new_is_culled;
}

bool ComponentMesh::IsCulled() const
{
	return is_culled;
}

void ComponentMesh::DrawInspectorContent()
{
	bool enabled_editor = Enabled();

	if (ImGui::Checkbox("Enabled", &enabled_editor))
	{
		enabled_editor ? Enable() : Disable();
	}
}

void ComponentMesh::LoadAABB()
{
	//TODO: Maybe there is a more efficient method to this. Find one.

	float3* temp_vertices = new float3[number_of_vertices];

	for (size_t i = 0; i < number_of_vertices * 8; i += 8)
	{
		temp_vertices[i / 8] = float3(vertices[i + 0], vertices[i + 1], vertices[i + 2]);
	}

	bounding_box.SetNegativeInfinity();

	bounding_box.SetFrom(temp_vertices, number_of_vertices);

	delete[] temp_vertices;
}
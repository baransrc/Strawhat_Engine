#include "ComponentMesh.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "GLEW/include/GL/glew.h"

#include "MATH_GEO_LIB/Geometry/Polyhedron.h"

#include "Entity.h"

ComponentMesh::ComponentMesh() : Component(),
								 vertices(nullptr),
								 indices(nullptr),
								 vertex_array_object(0),
								 vertex_buffer_object(0),
								 element_buffer_object(0),
								 bounding_box(),
								 number_of_vertices(0),
								 number_of_indices(0),
								 number_of_triangles(0),
								 is_currently_loaded(false)

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

void ComponentMesh::Load(float* new_vertices, unsigned int* new_indices, const unsigned int* new_texture_ids, size_t new_number_of_vertices, size_t new_number_of_indices, size_t new_number_of_triangles, size_t new_number_of_texture_ids)
{
	// TODO: Get this model_textures stuff from somewhere, do not hold this for each mesh component.
	number_of_texture_ids = new_number_of_texture_ids;
	texture_ids = (unsigned int*)malloc(sizeof(unsigned int) * number_of_texture_ids);
	for (size_t i = 0; i < number_of_texture_ids; ++i)
	{
		texture_ids[0] = new_texture_ids[0];
	}

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

	// Invoke change in parent:
	// TODO: This is ugly af, fix this, maybe we need some sub events?
	if (owner->Parent() != nullptr)
	{
		owner->Parent()->GetComponentsChangedInDescendantsEvent()->Invoke(Type());
	}
	owner->GetComponentsChangedEvent()->Invoke(Type());
}

void ComponentMesh::Update()
{
	App->shader_program->Use();

	// Activate Texture Unit 0:
	glActiveTexture(GL_TEXTURE0);
	// Bind Texture Unit 0:
	glBindTexture(GL_TEXTURE_2D, texture_ids[0]); // For now we are interested in only diffuse texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("input_texture", 0);

	// Bind VAO:
	glBindVertexArray(vertex_array_object);
	// Draw Mesh with VBO and EBO:
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, nullptr);
	// Unbind VAO:
	glBindVertexArray(0);
}

void ComponentMesh::Reset()
{
	is_currently_loaded = false;

	free(indices);
	free(vertices);
	free(texture_ids);

	indices = nullptr;
	vertices = nullptr;

	glDeleteBuffers(1, &element_buffer_object);
	glDeleteBuffers(1, &vertex_buffer_object);
	glDeleteVertexArrays(1, &vertex_array_object);
}

void ComponentMesh::DrawGizmo()
{
	App->debug_draw->DrawCuboid(bounding_box.ToPolyhedron().VertexArrayPtr(), math::float3(1.0f, 0.0f, 0.0f));
}

void ComponentMesh::DrawInspectorContent()
{
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
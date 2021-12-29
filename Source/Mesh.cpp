#include "Application.h"
#include "ModuleShaderProgram.h"
#include "Mesh.h"
#include "GLEW/include/GL/glew.h"

Mesh::Mesh()
{
	// No initialization is made here since everything 
	// is loaded with Mesh::Load
}

Mesh::~Mesh()
{
	free(indices);
	free(vertices);

	indices = nullptr;
	vertices = nullptr;

	glDeleteBuffers(1, &element_buffer_object);
	glDeleteBuffers(1, &vertex_buffer_object);
	glDeleteVertexArrays(1, &vertex_array_object);
}

/// <summary>
/// Loads mesh with given mesh_data.
/// User of this method must be aware of the fact that
/// this method calls Mesh::LoadMeshData internally
/// which does an allocation for vertices and indices.
/// It is strongly advised that, recurring use of this 
/// method may result in memory leak. 
/// So, please either delete this mesh and initialize it 
/// again, or use another Mesh.
/// </summary>
/// <param name="mesh_data"></param>
void Mesh::Load(const aiMesh* mesh_data)
{
	// Parse all the mesh data from Assimp mesh_data:
	LoadMeshData(mesh_data);

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

	// Load AABB:
	LoadAABB(mesh_data);
}

/// <summary>
/// Draws the mesh with given texture. For now, it only
/// uses the first texture in the array 'model_textures'
/// </summary>
/// <param name="model_textures"></param>
void Mesh::Draw(const unsigned int* model_textures) const
{
	// Activate Texture Unit 0:
	glActiveTexture(GL_TEXTURE0);
	// Bind Texture Unit 0:
	glBindTexture(GL_TEXTURE_2D, model_textures[0]); // For now we are interested in only diffuse texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("input_texture", 0);

	// Bind VAO:
	glBindVertexArray(vertex_array_object);
	// Draw Mesh with VBO and EBO:
	glDrawElements(GL_TRIANGLES, number_of_indices, GL_UNSIGNED_INT, nullptr);
	// Unbind VAO:
	glBindVertexArray(0);
}

/// <summary>
/// Allocates memory for vertices and indices and loads related mesh data
/// parsed from mesh_data into them.
/// </summary>
/// <param name="mesh_data"></param>
void Mesh::LoadMeshData(const aiMesh* mesh_data)
{
	// NOTE: Should this method free vertices and indices
	// if they are not nullptr before their calloc calls
	// to avoid memory leaks if the user calls Mesh::Load
	// again?

	number_of_vertices = mesh_data->mNumVertices;
	number_of_triangles = mesh_data->mNumFaces;
	
	vertices = (float*)calloc(number_of_vertices * 8, sizeof(float)); // Initializes to 0 by default.

	for (size_t i = 0; i < number_of_vertices * 8; i+=8)
	{
		const aiVector3D position = mesh_data->mVertices[i / 8];
		const aiVector3D normal = mesh_data->mNormals[i / 8];
		
		// Position:
		vertices[i + 0] = position.x; // NOTE: Yeah I'm adding +0 because I like consistency, please don't judge me.
		vertices[i + 1] = position.y;
		vertices[i + 2] = position.z;

		// Normal:
		vertices[i + 3] = normal.x;
		vertices[i + 4] = normal.y;
		vertices[i + 5] = normal.z;

		// Since all the vertices data is initialized to 0.0f by default,
		// if mesh_data->mTextureCoords[0] is null, leave texture coordinates 
		// as zero, and move to the next iteration.
		if (!mesh_data->mTextureCoords[0])
		{
			continue;
		}

		const aiVector3D texture_coordinate = mesh_data->mTextureCoords[0][i / 8];
		
		// Texture Coordinates:
		vertices[i + 6] = texture_coordinate.x;
		vertices[i + 7] = texture_coordinate.y;
	}

	size_t number_of_faces = mesh_data->mNumFaces;
	number_of_indices = number_of_faces * 3; // Assuming there are 3 vertices per triangle.

	indices = (unsigned int*)calloc(number_of_indices, sizeof(unsigned int)); // Initializes to 0 by default.

	for (size_t i = 0; i < number_of_faces; ++i)
	{
		indices[i * 3 + 0] = mesh_data->mFaces[i].mIndices[0]; // NOTE: It's me again, adding 0 for the pure sake of visual consistency.
		indices[i * 3 + 1] = mesh_data->mFaces[i].mIndices[1];
		indices[i * 3 + 2] = mesh_data->mFaces[i].mIndices[2];
	}
}

/// <summary>
/// Creates AABB from mesh_data->mVertices.
/// </summary>
/// <param name="mesh_data"></param>
void Mesh::LoadAABB(const aiMesh* mesh_data)
{
	// Since the memory layouts of float3 and aiVector3D this works.
	bounding_box = AABB::MinimalEnclosingAABB((float3*)(&(mesh_data->mVertices[0])), mesh_data->mNumVertices);
}

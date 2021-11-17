#include "Application.h"
#include "ModuleRenderExercise.h"
#include "ModuleShaderProgram.h"
#include "ModuleTexture.h"
#include "Util.h"

constexpr float vertices[] = {
    // positions          // colors           // texture coords
    -0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   // top right
     0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   // bottom right
	 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   // bottom left
    -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f    // top left 
};
constexpr unsigned int indices[] = {
		0, 1, 2, // first triangle
		2, 3, 0  // second triangle
};

ModuleRenderExercise::ModuleRenderExercise()
{

}

bool ModuleRenderExercise::Init()
{

	// Generate vertex array object:
	glGenVertexArrays(1, &vertex_array_object);
	// Generate buffer with a unique buffer ID and store it 
	// inside vertex_buffer_object:
	glGenBuffers(1, &vertex_buffer_object);
	// Generate element buffer object that stores indices of vertices 
	// to draw in order which is stored in our vertext_buffer_object:
	glGenBuffers(1, &element_buffer_object);

	// Bind vertex_array_object:
	glBindVertexArray(vertex_array_object);

	// Bind the newly created buffer to the GL_ARRAY_BUFFER target:
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	
	// From now on any buffer calls made will be used to configure 
	// the currently bound buffer (vertex_buffer_object).
	// The following function copies the previously defined vertex
	// data into the buffer's memory.
	// First parameter is the type of the buffer we want to copy data 
	// into and vertex_buffer_object is currently bound to 
	// the GL_ARRAY_BUFFER target. Second parameter is the size of the 
	// passed data in bytes. And obviously the third parameter is the
	// data sent. The fourth parameter specifies how we want the graphics
	// card to manage the given data and this can be three different forms,
	// GL_STREAM_DRAW: the data is set only once and used by the GPU at most
	// a few times.
	// GL_STATIC_DRAW: the data is set only once and use many times.
	// GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
	// Since the position of the triangle will not change in our case, and it
	// is used a lot, also stays same for every render call, the best usage 
	// would be the GL_STATIC_DRAW, and for that reason, it is used here:
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Bind element array buffer:
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_object);
	// Load indices data to element buffer object:
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// Tell OpenGL how to interpret the vertext data:
	int size_of_vertex_attribute = 3;
	GLenum type_of_data = GL_FLOAT;
	GLboolean should_be_normalized = GL_FALSE;
	int stride = 8 * sizeof(float);


	// Position Attribute:
	glVertexAttribPointer
	(
		0, 							// Attribute offset
		3, 							// Number of elements in attribute
		type_of_data, 				// Type of data
		should_be_normalized, 		// Should OpenGL Normalize it?
		stride, 					// Length of one vertex data
		(void*) 0					// Start position of this attribute in array
	);	

	//// Enable the vertex position attribute:
	glEnableVertexAttribArray(0);

	// Color Attribute:
	glVertexAttribPointer
	(
		1,							// Attribute offset
		3, 							// Number of elements in attribute
		type_of_data, 				// Type of data
		should_be_normalized, 		// Should OpenGL Normalize it?
		stride, 					// Length of one vertex data
		(void*)(3 * sizeof(float)) // Start position of this attribute in array
	);
	// Enable the vertex color attribute:
	glEnableVertexAttribArray(1);

	// Texture Attribute:
	glVertexAttribPointer
	(
		2,							// Attribute offset
		2, 							// Number of elements in attribute
		type_of_data, 				// Type of data
		should_be_normalized, 		// Should OpenGL Normalize it?
		stride, 					// Length of one vertex data
		(void*)(6 * sizeof(float)) // Start position of this attribute in array
	);
	// Enable the vertex texture attribute:
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	
	// Load Lena Texture:
	lena_texture = App->texture->LoadTexture(
		util::ConcatCStrings(App->GetWorkingDirectory(), LENA_TEXTURE_PATH), 
		GL_NEAREST,
		GL_NEAREST,
		GL_CLAMP_TO_EDGE,	
		GL_CLAMP_TO_EDGE,
		false,
		false	
	);

	// Use the shader program created in ModuleShaderProgram:
	App->shader_program->Use();
	App->shader_program->SetUniformVariable("input_texture", 0);

	return true;
}

bool ModuleRenderExercise::CleanUp()
{
	App->texture->UnloadTexture(&lena_texture, 1);

	glDeleteBuffers(1, &element_buffer_object);
	glDeleteBuffers(1, &vertex_buffer_object);
	glDeleteVertexArrays(1, &vertex_array_object);

	return true;
}


update_status ModuleRenderExercise::Update()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lena_texture);

	// Use the shader program created in ModuleShaderProgram:
	App->shader_program->Use();

	glBindVertexArray(vertex_array_object);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // NOTE: Should this be in PostUpdate?
	
    return update_status::UPDATE_CONTINUE;
}

ModuleRenderExercise::~ModuleRenderExercise()
{

}


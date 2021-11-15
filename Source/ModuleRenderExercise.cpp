#include "Application.h"
#include "ModuleRenderExercise.h"
#include "GLEW/include/GL/glew.h"

constexpr float vertices[] = {
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	 0.0f,  1.0f, 0.0f
};

ModuleRenderExercise::ModuleRenderExercise()
{

}

bool ModuleRenderExercise::Init()
{
	// Generate buffer with a unique buffer ID and store it 
	// inside vertex_buffer_object:
	glGenBuffers(1, &vertex_buffer_object);

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

	
	// Tell OpenGL how to interpret the vertext data:
	int offset = 0;
	int size_of_vertex_attribute = 3;
	GLenum type_of_data = GL_FLOAT;
	GLboolean should_be_normalized = GL_FALSE;
	int stride = size_of_vertex_attribute * sizeof(float);
	void* offset_to_the_first_attribute = (void*) 0;
	glVertexAttribPointer(offset, size_of_vertex_attribute, type_of_data, should_be_normalized, stride, offset_to_the_first_attribute);

	// Enable the vertex attribute:
	glEnableVertexAttribArray(0); // 0 is the index of the vertex attribute to be enabled.

	return true;
}

bool ModuleRenderExercise::CleanUp()
{
	glDeleteBuffers(1, &vertex_buffer_object);

	return true;
}


update_status ModuleRenderExercise::Update()
{
	glDrawArrays(GL_TRIANGLES, 0, 3); // NOTE: Should this be in PostUpdate?
	
    return update_status::UPDATE_CONTINUE;
}

ModuleRenderExercise::~ModuleRenderExercise()
{
	
}


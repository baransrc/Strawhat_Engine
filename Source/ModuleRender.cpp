#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleWindow.h"
#include "SDL.h"
#include "GLEW/include/GL/glew.h"
#include <random>
#include <iostream>
#include <iomanip>

ModuleRender::ModuleRender()
{
}

// Destructor
ModuleRender::~ModuleRender()
{
}

// Called before render is available
bool ModuleRender::Init()
{
	LOG("Creating Renderer context");
    
	// Initialize GLEW and OpenGL:
	InitializeGLEW();
    
	// Log Hardware Details:
	LogHardware();
    
	// Initialize Render Pipline Options According to the Settings in Globals.h:
	InitializeRenderPipelineOptions();
    
	// Random
	std::srand(std::time(nullptr));
    
	return true;
}

update_status ModuleRender::PreUpdate()
{
	// Resize the viewport to the newly resized window:
	glViewport(0, 0, App->window->window_width, App->window->window_height);
    
	// Change Clear Color
	glClearColor((float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, (float)(rand()) / RAND_MAX, 1.0f);
    
	// Clear to the selected Clear color:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
    
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
	// Swap frame buffer:
	SDL_GL_SwapWindow(App->window->window);
    
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRender::CleanUp()
{
	LOG("Destroying renderer");
    
	//Delete OpenGL Context:
	SDL_GL_DeleteContext(context);
    
	return true;
}

void ModuleRender::WindowResized(unsigned width, unsigned height)
{
}

void ModuleRender::InitializeGLEW()
{
	// Set Attributes:
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);  // desired version
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);    // we want a double buffer
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); // we want to have a depth buffer with 24 bits
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);    // we want to have a stencil buffer with 8 bits
    
	// Create OpenGL Context:
	// Using the window provided by ModuleWindow in App.
	context = SDL_GL_CreateContext(App->window->window);
    
	// Load GLEW:
	GLenum err = glewInit();
	// Check for errors:
	LOG("Using Glew %s", glewGetString(GLEW_VERSION)); // This should log GLEW version as 2.0
    
	// Configure OpenGL:
	glEnable(GL_DEPTH_TEST);    // Enable depth test
	glEnable(GL_CULL_FACE);     // Enable cull backward faces
	glFrontFace(GL_CCW);        // Front faces will be counter clockwise
}

void ModuleRender::LogHardware()
{
	LOG("HARDWARE DETAILS:");
	LOG("Vendor: %s", glGetString(GL_VENDOR));
	LOG("Renderer: %s", glGetString(GL_RENDERER));
	LOG("OpenGL version supported %s", glGetString(GL_VERSION));
	LOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void ModuleRender::InitializeRenderPipelineOptions()
{
	// Read Settings from Globals.h and Initialize Render Pipeline Options Accordingly:
	
	// Face Culling:
	if (RENDERER_CULL_FACE == true)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
    
	// Depth Test:
	if (RENDERER_DEPTH_TEST == true)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}
    
	// Scissor Test:
	if (RENDERER_SCISSOR_TEST == true)
	{
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
    
	// Stencil Test:
	if (RENDERER_STENCIL_TEST == true)
	{
		glEnable(GL_STENCIL_TEST);
	}
	else
	{
		glDisable(GL_STENCIL_TEST);
	}
}




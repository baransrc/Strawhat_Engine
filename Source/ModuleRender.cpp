#include "Application.h"

#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "ComponentCamera.h"
#include "Entity.h"

#include "ModelImporter.h"
#include "Globals.h"
#include "Util.h"

#include "MATH_GEO_LIB/Geometry/Polyhedron.h"
#include "SDL.h"
#include "GLEW/include/GL/glew.h"
#include "ComponentBoundingBox.h"
#include "Entity.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

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
	viewport_height = App->window->window_height;
	viewport_width = App->window->window_width;

	// Initialize GLEW and OpenGL:
	InitializeGLEW();

	// Log Hardware Details:
	LogHardware();

	// Initialize Render Pipline Options According to the Settings in Globals.h:
	InitializeRenderPipelineOptions();

	// Initialize OpenGL debug features and vao.
	InitializeOpenGL();

	// Initialize window_resized_event_listener:
	window_resized_event_listener = EventListener<unsigned int, unsigned int>(std::bind(&ModuleRender::HandleWindowResized, this, std::placeholders::_1, std::placeholders::_2));
	// Subscribe to window resized event of ModuleInput:
	App->input->GetWindowResizedEvent()->AddListener(&window_resized_event_listener);

	viewport_height = App->window->window_height;
	viewport_width = App->window->window_width;

	return true;
}

update_status ModuleRender::PreUpdate()
{
	// Resize the viewport to the newly resized window:
	glViewport(0, 0, viewport_width, viewport_height);

	// Clear to clear color:
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);

	// Clear to the selected Clear color:
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return update_status::UPDATE_CONTINUE;
}

// Called every draw update
update_status ModuleRender::Update()
{
	// Use the shader program created in ModuleShaderProgram:
	App->shader_program->Use();

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

	// TODO(baran): Move this into a private method.
	// Unsubscribe from file dropped event if it's not null:
	Event<const char*>* file_dropped_event = App->input->GetFileDroppedEvent();
	if (file_dropped_event != nullptr)
	{
		file_dropped_event->RemoveListener(&file_dropped_event_listener);
	}
	// TODO(baran): Move this into a private method.
	// Unsubscribe from window resized if it's not null:
	Event<unsigned int, unsigned int>* window_resized_event = App->input->GetWindowResizedEvent();
	if (window_resized_event != nullptr)
	{
		window_resized_event->RemoveListener(&window_resized_event_listener);
	}
  
	return true;
}

void ModuleRender::HandleWindowResized(unsigned int width, unsigned int height)
{
	viewport_width = width;
	viewport_height = height;
}

float ModuleRender::GetRequiredAxisTriadLength() const
{
	return App->camera->GetCamera()->GetFarPlaneDistance() * 0.75f;
}

void ModuleRender::OnEditor()
{
	// Define Settings:
	static bool smooth_lines = false;
	static bool cull_face = RENDERER_CULL_FACE;
	static bool depth_test = RENDERER_DEPTH_TEST;
	static bool scissor_test = RENDERER_SCISSOR_TEST;
	static bool stencil_test = RENDERER_STENCIL_TEST;

	// Set counter clockwise triangles as front facing:
	glFrontFace(GL_CCW);

	if (ImGui::CollapsingHeader("Renderer"))
	{
		ImGui::Text("Clear Color");
		ImGui::Separator();
		ImGui::PushItemWidth(200.0f);
		ImGui::ColorPicker3("Clear Color", &clear_color[0], ImGuiColorEditFlags_DisplayRGB);

		ImGui::Text("\n");
		ImGui::Text("Viewport Dimensions");
		ImGui::Separator();
		ImGui::Text("Width: %u", viewport_width);
		ImGui::Text("Height: %u", viewport_height);

		ImGui::Text("\n");
		ImGui::Text("Settings");
		ImGui::Separator();

		ImGui::PushID("smooth_lines");
		ImGui::Checkbox("Smooth Lines", &smooth_lines);
		ImGui::PopID();

		ImGui::PushID("cull_face");
		ImGui::Checkbox("Face Culling", &cull_face);
		ImGui::PopID();

		ImGui::PushID("depth_test");
		ImGui::Checkbox("Depth Test", &depth_test);
		ImGui::PopID();

		ImGui::PushID("scissor_test");
		ImGui::Checkbox("Scissor Test", &scissor_test);
		ImGui::PopID();

		ImGui::PushID("stencil_test");
		ImGui::Checkbox("Stencil Test", &stencil_test);
		ImGui::PopID();
	}

	// Apply Settings:
	smooth_lines ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);
	cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	scissor_test ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	stencil_test ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
}

void ModuleRender::OnPerformanceWindow() const
{
	int total_vram = 0;
	int free_vram = 0;

	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_vram);
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &free_vram);

	static const float total_vram_gib = (total_vram / 1024.f) / 1024.f;
	float free_vram_gib = (free_vram / 1024.f) / 1024.f;

	ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
	ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
	ImGui::Text("OpenGL version supported %s", glGetString(GL_VERSION));
	ImGui::Text("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	ImGui::Text("VRAM: %fMb", total_vram_gib);
	ImGui::Text("VRAM Usage: %fGiB", (total_vram_gib - free_vram_gib));
	ImGui::Text("Free VRAM: %fGiB", free_vram_gib);
}

void ModuleRender::InitializeOpenGL()
{
#ifdef _DEBUG
	// Enable OpenGL context debug flags:
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

	// Enable output callbacks:
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	// Set OpenGL debug message callback:
	glDebugMessageCallback(&util::OpenGLDebugCallback, nullptr);

	// Filter Notifications:
	glDebugMessageControl(GL_DONT_CARE,
		GL_DONT_CARE,
		GL_DONT_CARE,
		0,
		nullptr,
		true);
#endif //  DEBUG

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

	// Set counter clockwise triangles as front facing:
	glFrontFace(GL_CCW);
}
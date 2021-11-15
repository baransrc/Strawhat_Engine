#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

bool ModuleEditor::Init()
{
	LOG("Initializing Editor");
	InitializeDearImGui();

	return true;
}

bool ModuleEditor::CleanUp()
{
	LOG("Destroying Editor");

	UninitializeDearImGui();

	return true;
}

update_status ModuleEditor::Update()
{
	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::InitializeDearImGui()
{
	// Create Dear ImGui context:
	ImGui::CreateContext();

	// Initialize Dear ImGui for SDL2 and OpenGL3:
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, const_cast<void*>(App->renderer->GetContext())); // TODO: Find a better way.
	ImGui_ImplOpenGL3_Init();
}

void ModuleEditor::UninitializeDearImGui()
{
	// Shutdown Dear ImGui for OpenGL3 and SDL2:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	// Destroy Dear ImGui context:
	ImGui::DestroyContext();
}

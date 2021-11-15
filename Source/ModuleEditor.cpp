#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Util.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

ModuleEditor::ModuleEditor()
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	LOG("Initializing Editor");

	// Initialize License:
	util::ReadFile(util::ConcatCStrings(App->GetWorkingDirectory(), "\\..\\LICENSE"), &license_buffer);

	InitializeDearImGui();
    
	show_demo_window = true;
    
	return true;
}

bool ModuleEditor::CleanUp()
{
	LOG("Destroying Editor");
    
	UninitializeDearImGui();

	free(license_buffer);
    
	return true;
}

void ModuleEditor::DrawMainMenuBar()
{
	// Begin Drawing Main Menu Bar:
	if (ImGui::BeginMainMenuBar())
	{
		// Begin Drawing Help Menu:
		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("About"))
			{
				// If user clicks on About, trigger about window:
				show_about_window = true;
			}

			// End Drawing Help Menu:
			ImGui::EndMenu();
		}

		// End Drawing Main Menu Bar:
		ImGui::EndMainMenuBar();
	}
}

void ModuleEditor::DrawAboutWindow()
{
	// Begin About Window:
	ImGui::Begin("About", &show_about_window);
	
	ImGui::AlignTextToFramePadding();
	ImGui::TextWrapped("Strawhat Engine");
	ImGui::Separator();
	ImGui::TextWrapped("Strawhat Engine is being developed for UPC Master's degree in Advanced Programming for Videogames.\n");
	
	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Author(s)");
	ImGui::Separator();
	ImGui::TextWrapped("I. Baran Sürücü");
	
	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("LICENSE");
	ImGui::Separator();

	if (license_buffer == nullptr)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
		ImGui::TextWrapped("An error occured while fetching license.");
		ImGui::PopStyleColor();
	} 
	else
	{
		ImGui::TextWrapped(license_buffer);
	}

	// End About Window:
	ImGui::End();
}

update_status ModuleEditor::PreUpdate()
{
	// Start the Dear ImGui frame:
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
    
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	DrawMainMenuBar();

	if (show_about_window)
	{
		DrawAboutWindow();
	}

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}


void ModuleEditor::InitializeDearImGui()
{
	// Create Dear ImGui context:
	ImGui::CreateContext();
    
	// Initialize Dear ImGui for SDL2 and OpenGL3:
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

void ModuleEditor::UninitializeDearImGui()
{
	// Shutdown Dear ImGui for OpenGL3 and SDL2:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
    
	// Destroy Dear ImGui context:
	ImGui::DestroyContext();
}

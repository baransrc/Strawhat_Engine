#include "ModuleEditor.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"
#include "Util.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "Globals.h"

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
	char* license_file_path = util::ConcatCStrings(App->GetWorkingDirectory(), "\\..\\LICENSE");
	util::ReadFile(license_file_path, &license_buffer);
	free(license_file_path);

	InitializeDearImGui();

	// Reserve space for fps and ms graphs:
	ms_data.reserve(TIMER_BUFFER_LENGTH);
	fps_data.reserve(TIMER_BUFFER_LENGTH);
    
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
		// Begin Drawing Tools Menu:
		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console"))
			{
				// If user clicks on Console, trigger console window:
				show_console_window = true;
			}

			if (ImGui::MenuItem("Render Exercise"))
			{
				show_render_exercise_texture_info_window = true;
			}

			if (ImGui::MenuItem("Performance"))
			{
				show_performance_window = true;
			}

			// End Drawing Tools Menu:
			ImGui::EndMenu();
		}

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
	ImGui::TextWrapped("Repository");
	ImGui::Separator();
	ImGui::TextWrapped("Feel free to check the GitHub Repository of Strawhat Engine.");
	if (ImGui::Button("Open Repository in Browser", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
	{
		// If Button is clicked, open the repository link in browser:
		util::OpenLink(LINK_TO_REPOSITORY);
	}

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Author(s)");
	ImGui::Separator();
	ImGui::TextWrapped("I. Baran Sürücü");

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("Libraries");
	ImGui::Separator();
	ImGui::TextWrapped(LIBRARIES_USED);

	ImGui::TextWrapped("\n");
	ImGui::TextWrapped("License");
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

void ModuleEditor::DrawConsoleWindow()
{
	if (!show_console_window)
	{
		return;
	}

	// Begin Console Window:
	ImGui::Begin("Console", &show_console_window);

	console->ToImGuiText();

	// End Console Window:
	ImGui::End();
}

void ModuleEditor::DrawRenderExerciseTextureInfoWindow()
{
	if (show_render_exercise_texture_info_window)
	{
		ImGui::Begin("Render Exercise Texture", &show_render_exercise_texture_info_window);
		App->render_exercise->DrawTextureInfoContent();
		ImGui::End();
	}
}

void ModuleEditor::DrawPerformanceWindow()
{
	if (!show_performance_window)
	{
		if (!fps_data.empty())
		{
			ms_data.clear();
		}

		if (!ms_data.empty())
		{
			ms_data.clear();
		}

		return;
	}

	if (fps_data.size() == TIMER_BUFFER_LENGTH)
	{
		fps_data.erase(fps_data.begin());
	}
	fps_data.push_back(Time->FPS());

	if (ms_data.size() == TIMER_BUFFER_LENGTH)
	{
		ms_data.erase(ms_data.begin());
	}
	ms_data.push_back(Time->DeltaTimeMs());
	
	ImGui::Begin("Performance", &show_performance_window);
	
	char title[25];
	
	sprintf_s(title, 25, "Framerate %.1f", fps_data.back());
	ImGui::PlotHistogram("##framerate", &fps_data.front(), fps_data.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
	
	sprintf_s(title, 25, "Milliseconds %.1f", ms_data.back());
	ImGui::PlotHistogram("##milliseconds", &ms_data.front(), ms_data.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));

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

	DrawConsoleWindow();

	DrawRenderExerciseTextureInfoWindow();

	DrawPerformanceWindow();

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

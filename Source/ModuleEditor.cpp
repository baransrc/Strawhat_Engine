#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleRenderExercise.h"

#include "Util.h"
#include "Globals.h"

#include "Entity.h"
#include "Component.h"
#include "ComponentLight.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

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
	char* license_file_path = util::ConcatCStrings(App->GetWorkingDirectory(), "\\..\\LICENSE");
	util::ReadFile(license_file_path, &license_buffer);
	free(license_file_path);

	InitializeDearImGui();

	// Reserve space for fps and ms graphs:
	ms_data.reserve(TIMER_BUFFER_LENGTH);
	fps_data.reserve(TIMER_BUFFER_LENGTH);

	show_demo_window = true;

	// Entity related stuff, just for testing, will be deleted:
	base_entity = App->renderer->GetLoadedModel();

	Entity* light_entity = new Entity();
	light_entity->Initialize("Light");
	light_entity->SetParent(base_entity);
	ComponentLight* component_light = new ComponentLight();
	component_light->Initialize(light_entity);
	component_light->Load(light_type::POINT);

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
		// Begin Drawing File:
		if (ImGui::BeginMenu("File"))
		{
			// NOTE: Don't forget to add show flags here:
			if (ImGui::MenuItem("Close all windows"))
			{
				show_demo_window = false;
				show_about_window = false;
				show_console_window = false;
				show_render_exercise_texture_info_window = false;
				show_performance_window = false;
				show_module_settings_window = false;
				show_exit_popup = false;
				should_draw_inspector_window = false;
			}

			if (ImGui::MenuItem("Exit"))
			{
				show_exit_popup = true;
			}

			// End Drawing Tools Menu:
			ImGui::EndMenu();
		}

		// Begin Drawing Tools Menu:
		if (ImGui::BeginMenu("Tools"))
		{

			if (ImGui::MenuItem("Console"))
			{
				// If user clicks on Console, trigger console window:
				show_console_window = true;
			}

			if (ImGui::MenuItem("Modules"))
			{
				show_module_settings_window = true;
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

void ModuleEditor::DrawExitPopup()
{
	if (show_exit_popup)
	{
		ImGui::Begin("Exit", &show_exit_popup);
		ImGui::Text("Are you sure you want to close Strawhat Engine?");
		if (ImGui::Button("Yes, exit", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
		{
			show_exit_popup = false;
			should_exit_application = true;
		}
		if (ImGui::Button("Cancel", ImVec2(ImGui::GetContentRegionAvailWidth(), 0)))
		{
			show_exit_popup = false;
		}
		ImGui::End();
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
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
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
	ImGui::Text("\n");
	ImGui::Text("Hardware");
	App->renderer->OnPerformanceWindow();

	ImGui::End();
}

void ModuleEditor::DrawInspector()
{
	if (!should_draw_inspector_window)
	{
		return;
	}

	ImGui::PushID("Inspector##Window");
	ImGui::Begin("Inspector", &should_draw_inspector_window);
	// For now. Will be deleted after ModuleSceneManager is added:

	Entity* selected_entity = base_entity->selected_entity_in_hierarchy;

	if (selected_entity != nullptr)
	{
		ImGui::BeginGroup();
		ImGui::Text(selected_entity->Name().c_str());
		ImGui::Text("ID: "); ImGui::SameLine(); ImGui::Text(std::to_string(selected_entity->Id()).c_str());
		ImGui::EndGroup();

		// Show right click menu
		if (ImGui::BeginPopupContextItem("Inspector##Window"))
		{

			if (ImGui::BeginMenu("Add Component"))
			{
				if (ImGui::Selectable("Camera"))
				{
					selected_entity->AddComponent<ComponentCamera>();
				}
				if (ImGui::Selectable("Mesh"))
				{
					selected_entity->AddComponent<ComponentMesh>();
				}
				if (ImGui::Selectable("Material"))
				{
					selected_entity->AddComponent<ComponentMaterial>();
				}
				if (ImGui::Selectable("Light"))
				{
					selected_entity->AddComponent<ComponentLight>();
				}

				ImGui::EndMenu();
				
			}

			if (ImGui::Selectable("Other"))
			{

			}

			ImGui::EndPopup();
		}

		for (Component* component : selected_entity->Components())
		{
			component->DrawInspector();
		}
	}


	ImGui::End();
	ImGui::PopID();
}

void ModuleEditor::DrawModuleSettings()
{
	if (!show_module_settings_window)
	{
		return;
	}

	ImGui::Begin("Module Settings", &show_module_settings_window);

	App->renderer->OnEditor();


	if (ImGui::CollapsingHeader("Entity Experiment"))
	{
		base_entity->DrawEditor();
	}

	ImGui::End();
}

update_status ModuleEditor::PreUpdate()
{
	// Start the Dear ImGui frame:
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	//ImGuizmo::BeginFrame();
	//ImGuizmo::Enable(true);



	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{



	DrawMainMenuBar();

	if (show_about_window)
	{
		DrawAboutWindow();
	}

	DrawExitPopup();

	DrawConsoleWindow();

	DrawRenderExerciseTextureInfoWindow();

	DrawPerformanceWindow();

	DrawModuleSettings();

	DrawInspector();

	//ImGuiWindowFlags frameWindow_flags =
	//	ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
	//	ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
	//	ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	//ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGuiWindowClass* frameWindowClass = nullptr;
	//ImGui::SetNextWindowPos(viewport->WorkPos);
	//ImGui::SetNextWindowSize(viewport->WorkSize);
	//ImGui::SetNextWindowViewport(viewport->ID);

	//
	//// DockSpace
	//ImGuiIO& io = ImGui::GetIO();
	//if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
	//{



	//	ImGuiID dockspace_id = ImGui::GetID("DockSpace");
	//	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), 0);

	//	ImGui::Begin("Test", nullptr, frameWindow_flags);
	//	float4x4 view = App->camera->GetViewMatrix();
	//	float4x4 proj = App->camera->GetProjectionMatrix();
	//	float4x4 asdasd = float4x4::identity;

	//	view.Transpose();
	//	proj.Transpose();
	//	ImGuizmo::DrawGrid((float*)&view, (float*)&proj, (float*)&asdasd, 100.f);
	//	ImGuizmo::SetRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	//	ImGuizmo::SetDrawlist();
	//	ImGui::End();
	//}

	ImGui::Begin("Test");
	ImGui::Image((void*)(intptr_t)App->renderer->GetFramebufferTextureId(), ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT), ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//SDL_GL_MakeCurrent(App->window->window, App->renderer->context);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate()
{
	if (should_exit_application)
	{
		return update_status::UPDATE_STOP;
	}

	return update_status::UPDATE_CONTINUE;
}


void ModuleEditor::InitializeDearImGui()
{
	// Create Dear ImGui context:
	ImGui::CreateContext();

	//Activate docking feature
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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

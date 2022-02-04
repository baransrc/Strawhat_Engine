#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderExercise.h"

#include "Util.h"
#include "Globals.h"

#include "Scene.h"
#include "Entity.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentLight.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentCamera.h"

#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "MATH_GEO_LIB/Math/Quat.h"
#include "MATH_GEO_LIB/Math/float3x3.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
#define RAD_TO_DEG (180.0 / PI)

ModuleEditor::ModuleEditor() :
	Module(),
	current_imguizmo_transform_mode(ImGuizmo::WORLD),
	current_imguizmo_operation(ImGuizmo::TRANSLATE)
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
	if (!show_about_window)
	{
		return;
	}

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

	//// Get Window width and heights:
	//float window_width = (float)App->window->window_width;
	//float window_height = (float)App->window->window_height;

	//float final_width = (window_width * 15) * 0.01f;
	//float final_pos = window_width - final_width;

	//const ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(ImVec2(final_pos+1, 55));
	//ImGui::SetNextWindowSize(ImVec2(final_width, window_height-55));

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));

	ImGui::PushID("Inspector##Window");
	ImGui::Begin("Inspector", &should_draw_inspector_window, ImGuiWindowFlags_NoResize);
	// For now. Will be deleted after ModuleSceneManager is added:

	const Scene* current_scene = App->scene_manager->GetCurrentScene();
	Entity* selected_entity = current_scene->GetSelectedEntity();
	unsigned int root_entity_id = current_scene->GetRootEntity()->Id();

	if (selected_entity != nullptr && selected_entity->Id() != root_entity_id)
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
		
		bool is_entity_active = selected_entity->IsActive();
		if (ImGui::Checkbox("Active", &is_entity_active))
		{
			selected_entity->SetActive(is_entity_active);
		}

		for (Component* component : selected_entity->Components())
		{
			component->DrawInspector();
		}
	}

	ImGui::End();
	ImGui::PopID();
	ImGui::PopStyleColor();
}

void ModuleEditor::DrawImGuizmoModeWindow()
{
	//// Get Window width and heights:
	//float window_width = (float)App->window->window_width;
	//float window_height = (float)App->window->window_height;

	//const ImGuiViewport* viewport = ImGui::GetMainViewport();
	//ImGui::SetNextWindowPos(ImVec2(0,15));
	//ImGui::SetNextWindowSize(ImVec2(window_width, 40));

	
	ImGui::Begin("Transfrom Mode", 0, ImGuiWindowFlags_NoTitleBar);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.0f, 0.0f, 0.0f, 0.50f));

	if (ImGui::RadioButton("Translate", current_imguizmo_operation == ImGuizmo::TRANSLATE))
	{
		current_imguizmo_operation = ImGuizmo::TRANSLATE;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Rotate", current_imguizmo_operation == ImGuizmo::ROTATE))
	{
		current_imguizmo_operation = ImGuizmo::ROTATE;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Scale", current_imguizmo_operation == ImGuizmo::SCALE))
	{
		current_imguizmo_operation = ImGuizmo::SCALE;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("Local", current_imguizmo_transform_mode == ImGuizmo::LOCAL))
	{
		current_imguizmo_transform_mode = ImGuizmo::LOCAL;
	}

	ImGui::SameLine();

	if (ImGui::RadioButton("World", current_imguizmo_transform_mode == ImGuizmo::WORLD))
	{
		current_imguizmo_transform_mode = ImGuizmo::WORLD;
	}

	ImGui::PopStyleColor();
	ImGui::End();
}

void ModuleEditor::DrawHierarchy()
{
	if (!should_draw_hierarchy_window)
	{
		return;
	}
  
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.3f));
	ImGui::Begin("Hierarchy", &should_draw_hierarchy_window);
	
	App->scene_manager->DrawHierarchyEditor();
	
	ImGui::End();
	ImGui::PopStyleColor();
}

void ModuleEditor::DrawModuleSettings()
{
	if (!show_module_settings_window)
	{
		return;
	}

	ImGui::Begin("Module Settings", &show_module_settings_window);

	App->renderer->OnEditor();
	
	ImGui::End();
}

update_status ModuleEditor::PreUpdate()
{
	// Start the Dear ImGui frame:
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGui::SetNextWindowBgAlpha(0);
	ImGui::Begin("DockSpace Demo", 0, window_flags);
	// Submit the DockSpace
	ImGuiIO& io = ImGui::GetIO();

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::CaptureMouseFromApp(false);
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f));
	ImGui::PopStyleColor();

	//ImGui::CaptureMouseFromApp(false);
	
	DrawImGuizmo();

	SwitchImGuizmoOperationModeWithKeyboard();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawImGuizmoModeWindow();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawMainMenuBar();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawAboutWindow();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawExitPopup();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawConsoleWindow();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawRenderExerciseTextureInfoWindow();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawPerformanceWindow();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawModuleSettings();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawInspector();
	ImGui::SetNextWindowBgAlpha(0.5f);
	DrawHierarchy();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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

void ModuleEditor::TransformSelectedEntityWithImGuizmo()
{
	Entity* const selected_entity_in_hierarchy = App->scene_manager->GetCurrentScene()->GetSelectedEntity();

	if (selected_entity_in_hierarchy == nullptr)
	{
		return;
	}

	// Will be used to render the delta of transformation we did with
	// ImGuizmo:
	math::float4x4 delta_transform = math::float4x4::identity;

	// ImGuizmo is column-major like OpenGL as opposed to MathGeoLib we use
	// so we get the transposed version of all the matrices we have first:
	math::float4x4 model_matrix = selected_entity_in_hierarchy->Transform()->GetMatrix().Transposed();
	math::float4x4 view_matrix = App->camera->GetCamera()->GetViewMatrix().Transposed();
	math::float4x4 projection_matrix = App->camera->GetCamera()->GetProjectionMatrix().Transposed();

	// Manipulate the model_matrix with the transforms made with ImGuizmo gizmos:
	ImGuizmo::Manipulate
	(
		view_matrix.ptr(),			// Float array version of view_matrix
		projection_matrix.ptr(),	// Float array version of projection_matrix
		current_imguizmo_operation,	// Current ImGuizmo operation, either ROTATE, TRANSLATE or SCALE
		current_imguizmo_transform_mode,		// Current ImGuizmo operation matrix mode, either LOCAL or WORLD
		(float*)&model_matrix,		// Model matrix of the selected transform, but transposed for ImGuizmo
		(float*)&delta_transform	// Delta_transform for ImGuizmo to render what changed with it's fancy gizmos
	);

	if (ImGuizmo::IsUsing() && !delta_transform.IsIdentity())
	{
		math::float3 translation = math::float3::zero;
		math::float4x4 rotation = math::float4x4::identity;
		math::float3 scaling = math::float3::zero;

		// Transform according to the current imguizmo operation:
		switch (current_imguizmo_transform_mode)
		{
		case ImGuizmo::LOCAL:
		{
			// Transpose the delta_transform we got from ImGuizmo::Manipulate
			// and use it to calculate new resulting_local_matrix:
			math::float4x4 resulting_local_model_matrix =
				delta_transform.Transposed() * selected_entity_in_hierarchy->Transform()->GetLocalMatrix();
			// Decompose the matrix and feed translation, rotation and scaling:
			resulting_local_model_matrix.Decompose(translation, rotation, scaling);

			switch (current_imguizmo_operation)
			{
			default:
			case ImGuizmo::TRANSLATE:
			{
				selected_entity_in_hierarchy->Transform()->SetLocalPosition(translation);
			}
			break;

			case ImGuizmo::ROTATE:
			{
				selected_entity_in_hierarchy->Transform()->SetLocalRotation(math::Quat(rotation));
			}
			break;

			case ImGuizmo::SCALE:
			{
				selected_entity_in_hierarchy->Transform()->SetLocalScale(scaling);
			}
			break;
			}
		}
		break;

		default:
		case ImGuizmo::WORLD:
		{
			// Transpose the resulting model matrix we got from ImGuizmo
			// back again as they used transposed version of our matrix:
			math::float4x4 resulting_model_matrix = model_matrix.Transposed();
			// Decompose the matrix and feed translation, rotation and scaling:
			resulting_model_matrix.Decompose(translation, rotation, scaling);

			switch (current_imguizmo_operation)
			{
			default:
			case ImGuizmo::TRANSLATE:
			{
				selected_entity_in_hierarchy->Transform()->SetPosition(translation);
			}
			break;

			case ImGuizmo::ROTATE:
			{
				selected_entity_in_hierarchy->Transform()->SetRotation(math::Quat(rotation));
			}
			break;

			case ImGuizmo::SCALE:
			{
				selected_entity_in_hierarchy->Transform()->SetScale(scaling);
			}
			break;
			}
		}
		}
	}
}

void ModuleEditor::UninitializeDearImGui()
{
	// Shutdown Dear ImGui for OpenGL3 and SDL2:
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();

	// Destroy Dear ImGui context:
	ImGui::DestroyContext();
}

// TODO: MRG.
void ModuleEditor::DrawImGuizmo()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	TransformSelectedEntityWithImGuizmo();
}

void ModuleEditor::SwitchImGuizmoOperationModeWithKeyboard()
{
	if (ImGui::IsKeyPressed(90))
	{
		current_imguizmo_operation = ImGuizmo::TRANSLATE;
	}
	if (ImGui::IsKeyPressed(69))
	{
		current_imguizmo_operation = ImGuizmo::ROTATE;
	}
	if (ImGui::IsKeyPressed(82))
	{
		current_imguizmo_operation = ImGuizmo::SCALE;
	}
}

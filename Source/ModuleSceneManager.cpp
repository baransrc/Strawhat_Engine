#include "Application.h"

#include "ModuleSceneManager.h"
#include "ModuleInput.h"

#include "Scene.h"
#include "Entity.h"

#include "ModelImporter.h"


ModuleSceneManager::ModuleSceneManager() :
	Module(),
	current_scene(nullptr)
{
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init()
{
	LOG("Initialize: Module Scene Manager");

	// NOTE(Baran): For now this Module only creates 
	// one scene, and uses it as the current_scene. 
	// In the future this Module will also be responsible
	// for Loading scenes, switching between scenes, 
	// and saving the scenes.

	current_scene = new Scene();
	current_scene->Initialize();

	// Also, for now we set the current scene's main camera to nullptr
	// so that we render from ModuleCamera's camera:
	current_scene->SetMainCamera(nullptr);

	// Initialize file_dropped_event_listener:
	file_dropped_event_listener = EventListener<const char*>(std::bind(&ModuleSceneManager::HandleFileDropped, this, std::placeholders::_1));
	// Subscribe to file drop event of ModuleInput:
	App->input->GetFileDroppedEvent()->AddListener(&file_dropped_event_listener);

	return true;
}

bool ModuleSceneManager::CleanUp()
{
	LOG("CleanUp: Module Scene Manager");

	// NOTE(Baran): For now, we create a scene as 
	// current_scene, thus we delete it here, but 
	// in the future, current_scene will hold a 
	// reference to the current scene, therefore
	// deleting it here will cause errors. 
	// Be careful with that.
	delete current_scene;

	// TODO(baran): Move this into a private method.
	// Unsubscribe from file dropped event if it's 
	// not null:
	Event<const char*>* file_dropped_event = App->input->GetFileDroppedEvent();
	if (file_dropped_event != nullptr)
	{
		file_dropped_event->RemoveListener(&file_dropped_event_listener);
	}

	return true;
}

update_status ModuleSceneManager::PreUpdate()
{
	current_scene->PreUpdate();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update()
{
	current_scene->Update();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleSceneManager::PostUpdate()
{
	current_scene->PostUpdate();

	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneManager::HandleFileDropped(const char* file_name)
{
	Entity* loaded_entity = ModelImporter::Import(file_name);

	if (loaded_entity == nullptr)
	{
		return;
	}

	loaded_entity->SetParent(current_scene->GetRootEntity());
}

void ModuleSceneManager::DrawHierarchyEditor()
{
	DrawRecursiveEntityHierarchy(current_scene->GetRootEntity(), true);
}

Scene* const ModuleSceneManager::GetCurrentScene() const
{
	return current_scene;
}

void ModuleSceneManager::DrawRecursiveEntityHierarchy(Entity* entity, bool is_root_entity)
{
	// TODO(Baran): Refactor this code as it looks ugly a.f, and make 
	// it non-recursive if possible.

	if (entity == nullptr)
	{
		return;
	}

	static bool being_renamed = false;
	bool open = false;
	char main_id_buffer[64];
	sprintf(main_id_buffer, "ent##%u\0", entity->Id());

	ImGui::PushID(main_id_buffer);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

	if (entity->GetChildren().size() <= 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	Entity* selected_entity_in_hierarchy = current_scene->GetSelectedEntity();

	bool is_selected = selected_entity_in_hierarchy != nullptr && selected_entity_in_hierarchy->Id() == entity->Id();

	if (is_selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	open = ImGui::TreeNodeEx(entity->Name().c_str(), flags);

	// If Current node is clicked:
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		selected_entity_in_hierarchy = entity;
		current_scene->SetSelectedEntity(selected_entity_in_hierarchy);
	}

	is_selected = selected_entity_in_hierarchy != nullptr && selected_entity_in_hierarchy->Id() == entity->Id();
		
	// Rename text input area:
	if (is_selected && being_renamed)
	{
		ImGui::SetItemAllowOverlap();
		char buffer[1024] = "Rename To";
		char rename_id_buffer[64];
		sprintf(rename_id_buffer, "rn##%u\0", entity->Id());

		LOG("%s", rename_id_buffer);

		ImGui::SameLine();
		ImGui::PushID(rename_id_buffer);
		if (ImGui::InputTextWithHint("", entity->Name().c_str(), &buffer[0], 1024, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			entity->SetName(buffer);
			being_renamed = false;
		}
		ImGui::PopID();
	}

	// Right Click context menu:
	if (!is_root_entity && ImGui::BeginPopupContextItem(main_id_buffer))
	{
		if (ImGui::MenuItem("Add Empty"))
		{
			ImGui::CloseCurrentPopup();

			Entity* child = new Entity();

			child->Initialize("Empty");

			child->SetParent(entity);
		}

		if (ImGui::MenuItem("Delete"))
		{
			ImGui::CloseCurrentPopup();

			entity->SetParent(nullptr);

			delete entity;

			return;
		}

		if (ImGui::MenuItem("Rename"))
		{
			ImGui::CloseCurrentPopup();

			being_renamed = true;
		}

		ImGui::EndPopup();
	}

	if (open)
	{
		if (entity->GetChildren().size() > 0)
		{
			for (Entity* child : entity->GetChildren())
			{
				DrawRecursiveEntityHierarchy(child, false);
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();	
}
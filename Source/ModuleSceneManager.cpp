#include "Application.h"

#include "ModuleSceneManager.h"
#include "ModuleInput.h"

#include "Scene.h"
#include "Entity.h"

#include "Util.h"
#include "ModelImporter.h"


ModuleSceneManager::ModuleSceneManager() :
	Module(),
	current_scene(nullptr),
	renamed_entity_in_hierarchy(nullptr)
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

	bool being_renamed = renamed_entity_in_hierarchy != nullptr && renamed_entity_in_hierarchy->Id() == entity->Id();
	bool open = false;
	// should_be_deleted is set to true if delete was selected
	// and it gets deleted after the tree is rendered:
	bool should_be_deleted = false;
	// should_move_up_in_hierarchy is set to true if the current entity "Move Up"
	// is called:
	bool should_move_up_in_hierarchy = false;

	// Get a unique id of the current node for click events and etc to be unique to
	// that node:
	char main_id_buffer[64];
	sprintf(main_id_buffer, "ent##%u\0", entity->Id());

	ImGui::PushID(main_id_buffer);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

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

	open = ImGui::TreeNodeEx(being_renamed ? "" : entity->Name().c_str(), flags);

	// If Current node is clicked:
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		if (!being_renamed)
		{
			renamed_entity_in_hierarchy = nullptr;
		}

		selected_entity_in_hierarchy = entity;
		current_scene->SetSelectedEntity(selected_entity_in_hierarchy);
	}

	is_selected = selected_entity_in_hierarchy != nullptr && selected_entity_in_hierarchy->Id() == entity->Id();
		
	// Rename text input area:
	if (being_renamed)
	{
		ImGui::SetItemAllowOverlap();

		char buffer[1024] = "";
		util::CopyIntoBuffer(buffer, entity->Name().c_str(), 1024, entity->Name().size());

		char rename_id_buffer[64];
		sprintf(rename_id_buffer, "rn##%u\0", entity->Id());

		ImGui::SameLine();

		ImGui::AlignTextToFramePadding();

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 1.0f, 1.0f, 0.5f));
		//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
		ImGui::PushID(rename_id_buffer);

		if (ImGui::InputTextWithHint("", entity->Name().c_str(), &buffer[0], 1024, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_AlwaysOverwrite))
		{
			entity->SetName(buffer);
			renamed_entity_in_hierarchy = nullptr;
		}

		ImGui::PopID();
		//ImGui::PopStyleVar();
		ImGui::PopStyleColor();
	}

	// Right Click context menu:
	if (ImGui::BeginPopupContextItem(main_id_buffer))
	{
		if (ImGui::MenuItem("Add Empty"))
		{
			ImGui::CloseCurrentPopup();

			Entity* child = new Entity();

			child->Initialize("Empty Entity");

			child->SetParent(entity);
		}

		if (!is_root_entity)
		{
			const bool dont_show_move_up = entity->Id() == current_scene->GetRootEntity()->Id() || 
				(entity->Parent() != nullptr && entity->Parent()->Id() == current_scene->GetRootEntity()->Id());

			if (!dont_show_move_up && ImGui::MenuItem("Move Up"))
			{
				ImGui::CloseCurrentPopup();
				
				should_move_up_in_hierarchy = true;
			}

			if (ImGui::MenuItem("Delete"))
			{
				ImGui::CloseCurrentPopup();

				if (is_selected)
				{
					current_scene->SetSelectedEntity(current_scene->GetRootEntity());
				}

				should_be_deleted = true;
			}

			if (ImGui::MenuItem("Rename"))
			{
				ImGui::CloseCurrentPopup();

				renamed_entity_in_hierarchy = entity;
			}
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

	if (should_move_up_in_hierarchy)
	{
		Entity* previous_parent = entity->Parent();

		// This if check is unnecessary as it is prevented 
		// above but put here anyway to have it secured:
		if (previous_parent != nullptr)
		{
			entity->SetParent(previous_parent->Parent());
		}
	}

	if (should_be_deleted)
	{
		entity->SetParent(nullptr);

		delete entity;
	}
}
#include "Application.h"

#include "ModuleSceneManager.h"
#include "ModuleInput.h"

#include "Scene.h"
#include "Entity.h"
#include "ComponentCamera.h"

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

	// Also, for now we set the current scene's main camera should_render 
	// to false so that we render from ModuleCamera's camera:
	current_scene->GetMainCamera()->SetShouldRender(false);
	// NOTE(Baran): We will switch should render to true when we are on play 
	// mode in the future.

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
	// This is updated again according to whether the node is collapsed or 
	// expanded:
	bool open = false;
	// should_be_deleted is set to true if delete was selected
	// and it gets deleted after the tree is rendered:
	bool should_be_deleted = false;
	// should_move_up_in_hierarchy is set to true if the current entity "Move Up"
	// is called:
	bool should_move_up_in_hierarchy = false;
	// Dropped child will be accessed like this at the end of the function if it's set to a non-nullptr entity
	Entity* new_dropped_child_entity = nullptr;

	// Get a unique id of the current node for click events and etc to be unique to
	// that node:
	char main_id_buffer[64];
	sprintf(main_id_buffer, "ent##%u\0", entity->Id());

	// Set the flags of the node:
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
	// If there are no child entities don't draw the arrow and
	// disable being collapsable/expandable:
	if (entity->GetChildren().size() <= 0)
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	// Cache selected entity in the current scene so we don't have to
	// call the getter everytime:
	Entity* selected_entity_in_hierarchy = current_scene->GetSelectedEntity();
	bool is_selected = selected_entity_in_hierarchy != nullptr && selected_entity_in_hierarchy->Id() == entity->Id();

	// If the entity is selected, render it as selected node:
	if (is_selected)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	// Render the tree node of entity, return if it's expanded or not:
	open = ImGui::TreeNodeEx(being_renamed ? "" : entity->Name().c_str(), flags);

	ImGui::PushID(main_id_buffer);

	// If Current node is clicked:
	if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
	{
		if (!being_renamed)
		{
			renamed_entity_in_hierarchy = nullptr;
		}

		// Update is selected as the item is clicked:
		is_selected = true;
		// Update the cached one as well:
		selected_entity_in_hierarchy = entity;
		// Update selected entity in the scene hierarchy:
		current_scene->SetSelectedEntity(selected_entity_in_hierarchy);
	}

	// Set this as a drag and drop target:
	if (ImGui::BeginDragDropTarget())
	{
		// Accept payloads of the type "entity_in_hierarchy":
		ImGui::AcceptDragDropPayload("entity_in_hierarchy");

		// Get the current drag drop payload ImGui has:
		const ImGuiPayload* drag_drop_payload = ImGui::GetDragDropPayload();

		// Check if it's valid for us to process:
		const bool is_entity = drag_drop_payload->IsDataType("entity_in_hierarchy");
		const bool is_delivery = drag_drop_payload->IsDelivery();

		if (is_delivery && is_entity)
		{
			// Get the payload data as unsigned int*:
			unsigned int* drag_dropped_entity_id = ((unsigned int*)drag_drop_payload->Data);

			// Received ptr to id should not be null and Corresponding Entity 
			// to the received id should not be the same with target's id:
			if (drag_dropped_entity_id != nullptr && *drag_dropped_entity_id != entity->Id())	
			{
				// Find the entity in the scene as all the entities are descendants of the root node of
				// the scene:
				Entity* dropped_entity = current_scene->GetRootEntity()->FindDescendant(*drag_dropped_entity_id);
				
				// Avoid dragged entity to be dropped on an entity that is 
				// an ancestor of it:
				const bool is_dropped_entity_not_an_ancestor = 
					dropped_entity != nullptr &&
					!dropped_entity->HasDescendant(entity->Id());
				// Set the new_dropped_child_entity to this dropped_entity, 
				// this will be used at the end of the function, that is not
				// a recursive area, and gets added as a child to entity:
				new_dropped_child_entity = is_dropped_entity_not_an_ancestor ? dropped_entity : nullptr;
			}
		}

		ImGui::EndDragDropTarget();
	}

	// If this is not the root entity, set the node as 
	// drag and droppable in the hierarchy:
	if (!is_root_entity && ImGui::BeginDragDropSource())
	{
		// This was purely made bc ImGui wants const void*:
		unsigned int data[1] = { entity->Id() };

		// Set the drag and drop payload data and type, data is
		// the id of this dragged entity:
		ImGui::SetDragDropPayload("entity_in_hierarchy", data, 1);

		ImGui::EndDragDropSource();
	}

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
		ImGui::PushID(rename_id_buffer);

		if (ImGui::InputTextWithHint("", entity->Name().c_str(), &buffer[0], 1024, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_AlwaysOverwrite))
		{
			entity->SetName(buffer);
			renamed_entity_in_hierarchy = nullptr;
		}

		ImGui::PopID();
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

		// If the entity is the root entity of the scene
		// don't show rename, delete and move up options:
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

	// If expanded, draw all the children:
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

	// This means we have a new entity drag drop on our entity:
	if (new_dropped_child_entity != nullptr)
	{
		new_dropped_child_entity->SetParent(entity);
	}

	// This means move up option was selected on this entity:
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

	// This means delete option was selected on this entity:
	if (should_be_deleted)
	{
		entity->SetParent(nullptr);

		delete entity;
	}
}
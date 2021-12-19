#include "Entity.h"
#include "Component.h"
#include "DEAR_IMGUI/include/imgui.h"
#include "Globals.h"

// This is for experiments on component addition and viewing for now. 
// and will be deleted when ModuleSceneManager is added:
Entity* Entity::selected_entity_in_hierarchy = nullptr;

Entity::Entity() : name(""), active(false), id(0), parent(nullptr), being_renamed(false)
{
}

Entity::~Entity()
{
	for (Component* component : components)
	{
		delete component;
	}

	components.clear();

	for (Entity* child : children)
	{
		delete child;
	}

	children.clear();
}

/// <summary>
/// Initializes this Entity by defining it's name and id, and setting it as active.
/// </summary>
/// <param name="new_name">Name of this Entity.</param>
void Entity::Initialize(std::string new_name)
{
	name = new_name;
	active = true;
	id = GetCurrentId();

	LOG("Entity initialized with name: %s and id: %u", name.c_str(), id);
}

/// <summary>
/// Called by parent Entity::Update. Calls Component::Update for all the components 
/// attached and then calls Entity::Update for all the children.
/// </summary>
void Entity::Update()
{
	if (components.size() > 0)
	{
		for (Component* component : components)
		{
			component->Update();
		}
	}

	if (children.size() > 0)
	{
		for (Entity* child : children)
		{
			child->Update();
		}
	}
}

/// <summary>
/// Adds a component that has been created and Initialized by user somewhere else. Please note that, if this
/// method returns false, user should delete the component created, herself.
/// </summary>
/// <param name="component">Component to be added.</param>
/// <returns>True if component was added successfully, false if component could not be added.</returns>
bool Entity::AddComponent(Component* component)
{
	// If a component with same id is already owned by this Entity,
	// Don't add component again and halt the method:
	if (FindComponentById(component->Id()) != nullptr)
	{
		return false;
	}

	// TODO: check for type and maximum number of that entity should her owner have before adding.

	components.push_back(component);
}

/// <summary>
/// For now since there is no other Component type, this function will add an empty 
/// component to the Entity, which has a unique id. For now, if component with same
/// id is already inside the Entity, this function will not add that component to the 
/// Entity.
/// </summary>
/// <param name="component"></param>
void Entity::AddComponent(component_type type)
{
	Component* component = CreateComponent(type);

	if (component == nullptr)
	{
		return;
	}

	component->Initialize(this);

	AddComponent(component);
}

/// <summary>
/// Removes component from Entity and deletes it if such component with id exists in the Entity.
/// </summary>
/// <param name="component">Component to be searched inside components</param>
void Entity::RemoveComponent(Component* component)
{
	if (components.size() <= 0)
	{
		return;
	}

	std::vector<Component*>::const_iterator component_index;
	bool founded_component = false;

	for (component_index = components.begin(); component_index < components.end(); ++component_index)
	{
		if (component->Id() == (*component_index)->Id())
		{
			founded_component = true;
			break;
		}
	}

	if (founded_component)
	{
		delete component;
		components.erase(component_index);
	}
}

/// <summary>
/// Don't use this, not implemented yet.
/// </summary>
/// <param name="component"></param>
Component* Entity::CreateComponent(component_type type) const
{
	// This was added here since the slides had this sort of method
	// inside the Entity class.
	// TODO: Add Component creation here?

	// For now since there is no other Component type, this function will return an empty
	// component, with unique id.

	Component* component = new Component();

	return component;
}

/// <summary>
/// Don't use this, not implemented yet.
/// </summary>
/// <param name="component"></param>
Component* Entity::GetComponent(component_type type)
{
	// TODO: Add Get Component Functionality.

	return nullptr;
}

const std::vector<Component*>& Entity::GetComponents() const
{
	return components;
}

void Entity::SetName(std::string new_name)
{
	name = new_name;
}

/// <summary>
/// 
/// </summary>
/// <returns>Name of this Entity.</returns>
const std::string& Entity::Name() const
{
	return name;
}

/// <summary>
/// 
/// </summary>
/// <returns>Unique runtime id of this Entity.</returns>
unsigned int Entity::Id() const
{
	return id;
}

/// <summary>
/// 
/// </summary>
/// <returns>Parent of this Entity.</returns>
Entity* Entity::Parent() const
{
	return parent;
}

/// <summary>
/// Sets this Entity's parent to new_parent. It also removes this entity from it's previous parent if 
/// it's not nullptr, and adds this entity to the new_parent's children if new_parent is not nullptr.
/// </summary>
/// <param name="new_parent">Entity to be set as parent.</param>
void Entity::SetParent(Entity* new_parent)
{
	if (parent != nullptr)
	{
		parent->RemoveChild(this);
	}

	parent = new_parent;

	if (parent == nullptr)
	{
		return;
	}

	parent->AddChild(this);
}


/// <summary>
/// Defines if this Entity is active or not.
/// </summary>
/// <param name="activeness">True if Entity should be set as active, false if Entity should be set as disabled.</param>
void Entity::SetActive(bool activeness)
{
	active = activeness;
}

/// <summary>
/// Adds given child to this Entity if such child with same id is not in children already.
/// As this only adds child to the Entity and does not set it's parent and SetParent already
/// calls this method internally, use of child.SetParent with this Entity as parameter is advised. 
/// </summary>
/// <param name="child">Child to be added to children.</param>
void Entity::AddChild(Entity* child)
{
	if (FindChild(child->Id()) != nullptr)
	{
		return;
	}

	children.push_back(child);
}

/// <summary>
/// Removes child from this Entity if such child with id is found inside children.
/// </summary>
/// <param name="child">Child to be removed from children</param>
void Entity::RemoveChild(Entity* child)
{
	if (children.size() <= 0)
	{
		return;
	}

	std::vector<Entity*>::const_iterator child_index;
	bool founded_child = false;

	for (child_index = children.begin(); child_index < children.end(); ++child_index)
	{
		if (child->Id() == (*child_index)->Id())
		{
			founded_child = true;
			break;
		}
	}

	if (founded_child)
	{
		children.erase(child_index);
	}
}

/// <summary>
/// Finds child corresponding to ID "child_entity_id".
/// </summary>
/// <param name="child_entity_id">Id of the child that is being searched inside the entity.</param>
/// <returns>Child with ID "child_entity_id if" found, nullptr if there is no child with such ID.</returns>
Entity* Entity::FindChild(unsigned int child_entity_id) const
{
	if (children.size() > 0)
	{
		for (Entity* child : children)
		{
			if (child->Id() == child_entity_id)
			{
				return child;
			}
		}

	}

	return nullptr;
}

void Entity::DrawEditor()
{
	ImGui::PushID(id);

	bool open = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap);

	//// If current node is double clicked:
	//if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
	//{
	//	being_renamed = true;
	//}

	// If Current node is clicked:
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
	{
		selected_entity_in_hierarchy = this;
	}

	// Rename text input area:
	if (being_renamed)
	{
		ImGui::SetItemAllowOverlap();
		char buffer[1024] = "Rename To";
		char id_buffer[64];
		sprintf(id_buffer, "rn##%u\0");

		LOG("%s", id_buffer);

		ImGui::SameLine();
		ImGui::PushID(id_buffer);
		if (ImGui::InputTextWithHint("", name.c_str(), &buffer[0], 1024, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		{
			SetName(buffer);
			being_renamed = false;
		}
		ImGui::PopID();
	}

	// Right Click context menu:
	if (ImGui::BeginPopupContextItem())
	{
		if (ImGui::MenuItem("Add Empty"))
		{
			ImGui::CloseCurrentPopup();

			Entity* child = new Entity();

			child->Initialize("Empty");

			child->SetParent(this);
		}

		if (ImGui::MenuItem("Delete"))
		{
			ImGui::CloseCurrentPopup();

			SetParent(nullptr);

			delete this;
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
		if (children.size() > 0)
		{
			for (Entity* child : children)
			{
				child->DrawEditor();
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

/// <summary>
/// Used for giving an id to the Entity in Initialize method.
/// </summary>
/// <returns>A runtime unique id to the caller</returns>
unsigned int Entity::GetCurrentId()
{
	static unsigned int current_id = 0;

	return current_id++;
}

/// <summary>
/// Searches through the components vector if a component with such id exists inside components.
/// </summary>
/// <param name="id">Id of component to be found.</param>
/// <returns>The component if found, nullptr if not found.</returns>
Component* Entity::FindComponentById(unsigned int id) const
{
	for (Component* component : components)
	{
		if (component->Id() == id)
		{
			return component;
		}
	}

	return nullptr;
}
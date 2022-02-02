#include "ModuleSceneManager.h"

#include "Scene.h"
#include "Entity.h"

#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentBoundingBox.h"

#include "Globals.h"

Entity::Entity() : 
	name(""), 
	active(false), 
	id(0), 
	parent(nullptr), 
	components_changed(nullptr),
	components_changed_in_descendants(nullptr),
	hierarchy_changed(nullptr),
	transform(nullptr),
	bounding_box(nullptr)
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

	delete hierarchy_changed;
	delete components_changed;
	delete components_changed_in_descendants;
}

/// <summary>s
/// Initializes this Entity by defining it's name and id, and setting it as active.
/// </summary>
/// <param name="new_name">Name of this Entity.</param>
void Entity::Initialize(std::string new_name)
{
	name = new_name;
	active = true;
	id = GetCurrentId();

	components_changed = new Event<component_type>();
	components_changed_in_descendants = new Event<component_type>();
	hierarchy_changed = new Event<entity_operation>();

	// Initialize and add transform component:
	transform = new ComponentTransform();
	transform->Initialize(this);
	// Initialize and add bounding box components:
	bounding_box = new ComponentBoundingBox();
	bounding_box->Initialize(this);
	// NOTE: Initialize adds transform to components list of this entity.
	// So, no additional need to delete it separately, as it gets deleted
	// along with other components. Same goes with Update as well.
}

void Entity::PreUpdate()
{
	if (!active)
	{
		return;
	}

	if (components.size() > 0)
	{
		for (Component* component : components)
		{
			component->PreUpdate();
		}
	}

	if (children.size() > 0)
	{
		for (Entity* child : children)
		{
			child->PreUpdate();
		}
	}
}

/// <summary>
/// Called by parent Entity::Update. Calls Component::Update for all the components 
/// attached and then calls Entity::Update for all the children.
/// </summary>
void Entity::Update()
{
	if (!active)
	{
		return;
	}

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

void Entity::PostUpdate()
{
	if (!active)
	{
		return;
	}

	if (components.size() > 0)
	{
		for (Component* component : components)
		{
			component->PostUpdate();
		}
	}

	if (children.size() > 0)
	{
		for (Entity* child : children)
		{
			child->PostUpdate();
		}
	}
}

void Entity::DrawGizmos()
{
	for (Component* component : components)
	{
		component->DrawGizmo();
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

	bool component_of_type_already_exists = GetComponent(component->Type()) != nullptr;

	// If Component of type already exists and an Entity cannot have more than one of this
	// component at the same time, Don't add component and halt the method:
	if (component_of_type_already_exists && !Component::CanBeMoreThanOne(component->Type()))
	{
		return false;
	}

	components.push_back(component);

	// Trigger components changed events:
	InvokeComponentsChangedEvents(component->Type());

	return true;
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
		InvokeComponentsChangedEvents(component->Type());

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
/// Get the first component of type type. Prefer using GetComponent<>() if
/// the type is known beforehand to keep your code clean.
/// </summary>
/// <param name="type">Type of component</param>
/// <returns>First Component found in given type if it exists, nullptr if not.</returns>
Component* const Entity::GetComponent(component_type type)
{
	for (Component* component : components)
	{
		if (component->Type() == type)
		{
			return component;
		}
	}

	return nullptr;
}

const std::vector<Entity*>& Entity::GetChildren() const
{
	return children;
}

const std::vector<Component*>& Entity::Components() const
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

bool Entity::IsActive() const
{
	return active;
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

	hierarchy_changed->Invoke(entity_operation::PARENT_CHANGED);
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

	hierarchy_changed->Invoke(entity_operation::CHILDREN_CHANGED);
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

	InvokeChildHierarchyChangedEventRecursively();
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

bool Entity::HasDescendant(unsigned int descendant_entity_id) const
{
	return FindDescendant(descendant_entity_id) != nullptr;
}

Entity* Entity::FindDescendant(unsigned int descendant_entity_id) const
{
	if (children.size() > 0)
	{
		for (Entity* child : children)
		{
			if (child->Id() == descendant_entity_id)
			{
				return child;
			}

			Entity* descendant = child->FindDescendant(descendant_entity_id);

			if (descendant != nullptr)
			{
				return descendant;
			}
		}
	}

	return nullptr;
}

std::vector<Entity*> Entity::GetAllDescendants() const
{
	return std::vector<Entity*>();
}

void Entity::InvokeChildHierarchyChangedEventRecursively() const
{
	hierarchy_changed->Invoke(entity_operation::CHILDREN_CHANGED);
	
	if (parent != nullptr)
	{
		parent->InvokeChildHierarchyChangedEventRecursively();
	}
}

void Entity::InvokeComponentsChangedEvents(component_type type) const
{
	components_changed->Invoke(type);

	if (parent != nullptr)
	{
		parent->components_changed_in_descendants->Invoke(type);
	}
}

Event<component_type>* const  Entity::GetComponentsChangedEvent() const
{
	return components_changed;
}

Event<component_type>* const Entity::GetComponentsChangedInDescendantsEvent() const
{
	return components_changed_in_descendants;
}

Event<entity_operation>* const Entity::GetHierarchyChangedEvent() const
{
	return hierarchy_changed;
}

ComponentTransform* const Entity::Transform() const
{
	return transform;
}

ComponentBoundingBox* const Entity::BoundingBox() const
{
	return bounding_box;
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
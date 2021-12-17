#include "Entity.h"
#include "Component.h"

Entity::Entity() : name(""), active(false), id(0), parent(nullptr)
{
}

Entity::~Entity()
{
	// TODO: Firstly, delete all the components inside this Entity.
	// TODO: Secondly, delete all the children of this Entity.
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
/// Don't use this, not implemented yet.
/// </summary>
/// <param name="component"></param>
void Entity::AddComponent(Component* component)
{
	// TODO: Initialize given component and add it to components vector.
}

/// <summary>
/// Don't use this, not implemented yet.
/// </summary>
/// <param name="component"></param>
void Entity::AddComponent(component_type type)
{
	// TODO: Create component from given type, initialize it and add it to components vector.
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

	return nullptr;
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

	for (child_index = children.begin(); child_index < children.end(); ++child_index)
	{
		if (child->Id() == (*child_index)->Id())
		{
			break;
		}
	}

	children.erase(child_index);
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

/// <summary>
/// Used for giving an id to the Entity in Initialize method.
/// </summary>
/// <returns>A runtime unique id to the caller</returns>
unsigned int Entity::GetCurrentId()
{
	static unsigned int current_id = 0;

	return current_id++;
}
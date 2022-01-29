#pragma once

#include <vector>
#include <string>
#include "ComponentType.h"
#include "Event.h"
#include "Globals.h"

class Component;
class ComponentTransform;

class Entity
{
private:
	ComponentTransform* transform;
	std::vector<Component*> components;
	std::vector<Entity*> children;
	Event<component_type>* components_changed;
	Event<component_type>* components_changed_in_descendants;
	Entity* parent;
	std::string name;
	unsigned int id;
	bool active;

public:
	Entity();
	~Entity();

	void Initialize(std::string new_name);
	void PreUpdate();
	void Update();
	void PostUpdate();

	void DrawGizmos();

	bool AddComponent(Component* component);
	void RemoveComponent(Component* component);
	Component* CreateComponent(component_type type) const;
	const std::vector<Entity*>& GetChildren() const;

	template <class COMPONENT_TYPE>
	TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, void) AddComponent();
	template <class COMPONENT_TYPE> 
	TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, COMPONENT_TYPE* const) GetComponent() const;
	template <class COMPONENT_TYPE>
	TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) GetComponents() const;
	template <class COMPONENT_TYPE>
	TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) GetComponentsInChildren() const;
	template <class COMPONENT_TYPE>
	TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) GetComponentsIncludingChildren() const;

	Component* const GetComponent(component_type type);

	const std::vector<Component*>& Components() const;

	void SetName(std::string new_name);
	const std::string& Name() const;
	unsigned int Id() const;
	Entity* Parent() const;
	void SetParent(Entity* new_parent);
	void SetActive(bool activeness);
	void AddChild(Entity* child);
	void RemoveChild(Entity* child);
	Entity* FindChild(unsigned int child_entity_id) const;

	void InvokeComponentsChangedEvents(component_type type) const;
	Event<component_type>* const GetComponentsChangedEvent() const;
	Event<component_type>* const GetComponentsChangedInDescendantsEvent() const;

	ComponentTransform* const Transform() const;

private:
	Component* FindComponentById(unsigned int id) const;
	unsigned int GetCurrentId();
};

template<class COMPONENT_TYPE>
inline TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, void) Entity::AddComponent()
{
	COMPONENT_TYPE* component = new COMPONENT_TYPE();

	if (component == nullptr)
	{
		return;
	}

	bool added_successfully = AddComponent(component);

	if (!added_successfully)
	{
		delete component;
	}
	else
	{
		component->Initialize(this);
	}
}

template<class COMPONENT_TYPE>
inline TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, COMPONENT_TYPE* const) Entity::GetComponent() const
{
	component_type type = COMPONENT_TYPE().Type();

	for (Component* component : components)
	{
		if (component->Type() == type)
		{
			return (COMPONENT_TYPE*)component;
		}
	}

	return nullptr;
}

template<class COMPONENT_TYPE>
inline TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) Entity::GetComponents() const
{
	component_type type = COMPONENT_TYPE().Type();

	std::vector<COMPONENT_TYPE*> components_of_type;

	components_of_type.reserve(components.size());

	for (Component* component : components)
	{
		if (component->Type() == type)
		{
			components_of_type.push_back((COMPONENT_TYPE*) component);

			// If there is meant to be only one Component of this type
			// in the Entity, break out of the loop once a Component of
			// this type is found:
			if (!Component::CanBeMoreThanOne(type))
			{
				break;
			}

		}
	}

	return components_of_type;
}

template<class COMPONENT_TYPE>
inline TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) Entity::GetComponentsInChildren() const
{
	std::vector<COMPONENT_TYPE*> components_in_children;

	for (Entity* child : children)
	{
		std::vector<COMPONENT_TYPE*> components_in_child = child->GetComponents<COMPONENT_TYPE>();

		for (COMPONENT_TYPE* component : components_in_child)
		{
			components_in_children.push_back(component);
		}
	}

	return components_in_children;
}


template<class COMPONENT_TYPE>
inline TYPE_IF_DERIVED_CLASS(Component, COMPONENT_TYPE, std::vector<COMPONENT_TYPE*>) Entity::GetComponentsIncludingChildren() const
{
	std::vector<COMPONENT_TYPE*> components_including_children = GetComponents<COMPONENT_TYPE>();

	for (Entity* child : children)
	{
		std::vector<COMPONENT_TYPE*> components_in_child = child->GetComponents<COMPONENT_TYPE>();

		for (COMPONENT_TYPE* component : components_in_child)
		{
			components_including_children.push_back(component);
		}
	}

	return components_including_children;
}
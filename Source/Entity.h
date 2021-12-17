#pragma once

#include <vector>
#include <string>
#include "ComponentType.h"

class Component;

class Entity
{
private:
	std::vector<Component*> components;
	std::vector<Entity*> children;
	Entity* parent;
	std::string name;
	unsigned int id;
	bool active;

public:
	Entity();
	~Entity();

	void Initialize(std::string new_name);
	void Update();

	void AddComponent(Component* component);
	void AddComponent(component_type type);
	Component* CreateComponent(component_type type) const;
	Component* GetComponent(component_type type);
	
	const std::string& Name() const;
	unsigned int Id() const;
	Entity* Parent() const;
	void SetParent(Entity* new_parent);
	void SetActive(bool activeness);
	void AddChild(Entity* child);
	void RemoveChild(Entity* child);
	Entity* FindChild(unsigned int child_entity_id) const;

private:
	unsigned int GetCurrentId();
};
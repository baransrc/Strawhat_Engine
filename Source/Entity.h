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
	int id;
	bool active;

public:
	Entity();
	~Entity();

	void Update();

	void AddComponent(Component* component);
	void AddComponent(component_type type);
	Component* CreateComponent(component_type type) const;
	Component* GetComponent(component_type type);

	const Entity* GetParent() const;
	void AddChild(const Entity* child);
};
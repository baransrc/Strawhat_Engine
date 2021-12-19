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
	bool being_renamed;

public:
	static Entity* selected_entity_in_hierarchy; // This is for experiments on component addition and viewing for now. 
												 // and will be deleted when ModuleSceneManager is added.

public:
	Entity();
	~Entity();

	void Initialize(std::string new_name);
	void Update();

	bool AddComponent(Component* component);
	void AddComponent(component_type type);
	void RemoveComponent(Component* component);
	Component* CreateComponent(component_type type) const;
	Component* GetComponent(component_type type);


	const std::vector<Component*>& GetComponents() const;

	void SetName(std::string new_name);
	const std::string& Name() const;
	unsigned int Id() const;
	Entity* Parent() const;
	void SetParent(Entity* new_parent);
	void SetActive(bool activeness);
	void AddChild(Entity* child);
	void RemoveChild(Entity* child);
	Entity* FindChild(unsigned int child_entity_id) const;
	void DrawEditor();

private:
	Component* FindComponentById(unsigned int id) const;
	unsigned int GetCurrentId();
};
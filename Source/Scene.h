#pragma once

#include "Event.h"
#include "ComponentType.h"

#include "Entity.h"

#include <vector>

class Entity;
class ComponentCamera;
class ComponentMesh;

class Scene
{
private:
	ComponentCamera*				main_camera;
	Entity*							selected_entity;
	Entity*							root_entity;
	EventListener<component_type>	components_changed_in_descendants_event_listener;
	EventListener<entity_operation>	hierarchy_changed_event_listener;
	std::vector<ComponentMesh*>		mesh_components_in_scene;

public:
	Scene();
	~Scene();

	ComponentCamera* const GetMainCamera() const;
	Entity* const GetSelectedEntity() const;
	Entity* const GetRootEntity() const;

	void SetMainCamera(ComponentCamera* new_main_camera);
	void SetSelectedEntity(Entity* new_selected_entity);

	void CullMeshes();

	void Initialize();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void Delete();

private:
	void HandleHierarchyChangedEvent(entity_operation operation);
	void HandleComponentsChangedInDescendantsOfRoot(component_type type);
};
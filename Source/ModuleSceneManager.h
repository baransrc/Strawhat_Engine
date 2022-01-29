#pragma once

#include "Module.h"
#include "Event.h"

class Scene;
class Entity;

class ModuleSceneManager : public Module
{
private:
	Scene* current_scene; // For now we only have one scene support, in the future
						  // we may add an additional vector that holds all the scenes
						  // in the game, and add a load scene functionality as well.
	EventListener<const char*> file_dropped_event_listener;

public:
	ModuleSceneManager();
	~ModuleSceneManager() override;

	bool Init() override;
	bool CleanUp() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void HandleFileDropped(const char* file_name);
	void DrawHierarchyEditor();

	Scene* const GetCurrentScene() const;

private:
	void DrawRecursiveEntityHierarchy(Entity* entity, bool is_root_entity);
};
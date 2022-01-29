#pragma once

class Entity;
class ComponentCamera;

class Scene
{
private:
	ComponentCamera*	main_camera;
	Entity*				selected_entity;
	Entity*				root_entity;

public:
	Scene();
	~Scene();

	ComponentCamera* const GetMainCamera() const;
	Entity* const GetSelectedEntity() const;
	Entity* const GetRootEntity() const;

	void SetMainCamera(ComponentCamera* new_main_camera);
	void SetSelectedEntity(Entity* new_selected_entity);

	void Initialize();
	void PreUpdate();
	void Update();
	void PostUpdate();
	void Delete();
};
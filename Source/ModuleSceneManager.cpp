#include "Application.h"

#include "ModuleSceneManager.h"
#include "ModuleInput.h"

#include "Scene.h"
#include "Entity.h"

#include "ModelImporter.h"


ModuleSceneManager::ModuleSceneManager() :
	Module(),
	current_scene(nullptr)
{
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init()
{
	// NOTE(Baran): For now this Module only creates 
	// one scene, and uses it as the current_scene. 
	// In the future this Module will also be responsible
	// for Loading scenes, switching between scenes, 
	// and saving the scenes.

	current_scene = new Scene();
	current_scene->Initialize();

	// Also, for now we set the current scene's main camera to nullptr
	// so that we render from ModuleCamera's camera:
	current_scene->SetMainCamera(nullptr);

	// Initialize file_dropped_event_listener:
	file_dropped_event_listener = EventListener<const char*>(std::bind(&ModuleSceneManager::HandleFileDropped, this, std::placeholders::_1));
	// Subscribe to file drop event of ModuleInput:
	App->input->GetFileDroppedEvent()->AddListener(&file_dropped_event_listener);

	return true;
}

bool ModuleSceneManager::CleanUp()
{
	// NOTE(Baran): For now, we create a scene as 
	// current_scene, thus we delete it here, but 
	// in the future, current_scene will hold a 
	// reference to the current scene, therefore
	// deleting it here will cause errors. 
	// Be careful with that.
	delete current_scene;

	// TODO(baran): Move this into a private method.
	// Unsubscribe from file dropped event if it's 
	// not null:
	Event<const char*>* file_dropped_event = App->input->GetFileDroppedEvent();
	if (file_dropped_event != nullptr)
	{
		file_dropped_event->RemoveListener(&file_dropped_event_listener);
	}

	return true;
}

update_status ModuleSceneManager::PreUpdate()
{
	current_scene->PreUpdate();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update()
{
	current_scene->Update();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleSceneManager::PostUpdate()
{
	current_scene->PostUpdate();

	return update_status::UPDATE_CONTINUE;
}

void ModuleSceneManager::HandleFileDropped(const char* file_name)
{
	Entity* loaded_entity = ModelImporter::Import(file_name);

	if (loaded_entity == nullptr)
	{
		return;
	}

	loaded_entity->SetParent(loaded_entity);
}

Scene* const ModuleSceneManager::GetCurrentScene() const
{
	return current_scene;
}
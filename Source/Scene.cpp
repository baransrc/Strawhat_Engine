#include "Scene.h"

#include "Entity.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentTransform.h"

#include <vector>

Scene::Scene() :
    root_entity(nullptr),
    selected_entity(nullptr),
    main_camera(nullptr)
{
}

Scene::~Scene()
{
    delete root_entity;
}

ComponentCamera* const Scene::GetMainCamera() const
{
    return main_camera;
}

Entity* const Scene::GetSelectedEntity() const
{
    return selected_entity;
}

Entity* const Scene::GetRootEntity() const
{
    return root_entity;
}

void Scene::SetMainCamera(ComponentCamera* new_main_camera)
{
    main_camera = new_main_camera;
    
    std::vector<ComponentCamera*> cameras_in_scene = root_entity->GetComponentsInChildren<ComponentCamera>();

    for (ComponentCamera* camera : cameras_in_scene)
    {
        if (main_camera != nullptr && camera->Id() == main_camera->Id())
        {
            continue;
        }

        camera->SetIsMainCamera(false);
    }

    // NOTE(Baran): If we ever add any play mode/editor mode switching,
    // the main camera should always be the one that ModuleCamera
    // has while we ara in the editor mode.
}

void Scene::SetSelectedEntity(Entity* new_selected_entity)
{
    selected_entity = new_selected_entity;
}

void Scene::Initialize()
{
    // If the scene is initialized before, delete all the
    // scene data before doing anything:
    Delete();
    
    // Initialize the root_entity:
    root_entity = new Entity();
    root_entity->Initialize("Root Entity");
    
    // Add an entity with a camera component to the root
    // entity:
    Entity* camera_entity = new Entity();
    camera_entity->Initialize("Main Camera");
    camera_entity->SetParent(root_entity);
    camera_entity->AddComponent<ComponentCamera>();
    // Set camera_entity's camera component as the main_camera:
    SetMainCamera(camera_entity->GetComponent<ComponentCamera>());
    main_camera->SetIsMainCamera(true);
    // Set camera_entity's position:
    camera_entity->Transform()->SetPosition(math::float3(0.0f, 10.0f, 10.0f));

    // Add an entity with a light component as a child of root_entity:
    Entity* light_entity = new Entity();
    light_entity->Initialize("Directional Light");
    light_entity->SetParent(root_entity);
    light_entity->AddComponent<ComponentLight>();
    light_entity->GetComponent<ComponentLight>()->Load(light_type::DIRECTIONAL);

    // Set selected entity as the light_entity:
    // NOTE: If new stuff is added into SetSelectedEntity, 
    // just use that instead of this.
    selected_entity = light_entity;
}

void Scene::PreUpdate()
{
    root_entity->PreUpdate();
}

void Scene::Update()
{
    root_entity->Update();

    if (selected_entity != nullptr)
    {
        selected_entity->DrawGizmos();
    }
}

void Scene::PostUpdate()
{
    root_entity->PostUpdate();
}

void Scene::Delete()
{
    if (root_entity != nullptr)
    {
        delete root_entity;
    }

    root_entity = nullptr;

    selected_entity = nullptr;

    main_camera = nullptr;
}
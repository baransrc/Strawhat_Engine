#include "Scene.h"

#include "Entity.h"
#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"

#include "MATH_GEO_LIB/Geometry/Triangle.h"
#include "MATH_GEO_LIB/Geometry/LineSegment.h"


Scene::Scene() :
    root_entity(nullptr),
    selected_entity(nullptr),
    main_camera(nullptr),
    components_changed_in_descendants_event_listener(EventListener<component_type>())
{
}

Scene::~Scene()
{   
    // Since root_entity delete will call the 
    // remove all listeners method of the event(s)
    // we have subscribed here, no need to unsubscribe
    // before we delete the root entity:

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
    // has while we are in the editor mode.
}

void Scene::SetSelectedEntity(Entity* new_selected_entity)
{
    selected_entity = new_selected_entity;
}

void Scene::CullMeshes()
{
    for (ComponentMesh* mesh : mesh_components_in_scene)
    {
        // TODO: Add component bounding box to all entities:
        ComponentBoundingBox* bounding_box = 
            mesh->Owner()->GetComponent<ComponentBoundingBox>();

        if (bounding_box == nullptr)
        {
            continue;
        }

        math::OBB obb = bounding_box->GetBoundingBox();

        mesh->SetCulled(!main_camera->DoesOBBHavePointInsideFrustum(obb));
    }
}

void Scene::Initialize()
{
    // If the scene is initialized before, delete all the
    // scene data before doing anything:
    Delete();
    
    // Initialize the root_entity:
    root_entity = new Entity();
    root_entity->Initialize("Root Entity");

    // Subscribe to the components_changed_in_descendants event 
    // of root_entity:
    components_changed_in_descendants_event_listener = 
        EventListener<component_type>
        (
            std::bind
            (
                &Scene::HandleComponentsChangedInDescendantsOfRoot, 
                this, 
                std::placeholders::_1
            )
        );

    root_entity->GetComponentsChangedInDescendantsEvent()
        ->AddListener(&components_changed_in_descendants_event_listener);
    
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
    camera_entity->Transform()->SetPosition(math::float3(0.0f, 50.0f, 0.0f));
    main_camera->LookAt(math::float3(0.0f, 50.0f, 0.0f));

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

    // TODO: Move this inside HandleComponentsChangedInDescendantsOfRoot
    // For now we call this here bc we draw gizmos of non culled meshes
    CullMeshes();
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

void Scene::HandleComponentsChangedInDescendantsOfRoot(component_type type)
{
    // Right now we will only care about changes in transform, 
    // mesh and bounding box components of descendants, as
    // they will change the culled entities:
    switch (type)
    {
        case component_type::TRANSFORM:
            break;
        case component_type::MESH:
            break;
        case component_type::BOUNDING_BOX:
            break;
        default:
            return;
    }
    // NOTE(Baran): Add here to make all camera should_render false if new
    // camera changes are discovered here and we are in editor mode.
    
    // Get mesh components in scene:
    mesh_components_in_scene.clear();
    mesh_components_in_scene = (root_entity->GetComponentsInDescendants<ComponentMesh>());
}

void Scene::CheckRaycast(LineSegment segment) 
{
    std::vector <Entity*> entities;
    float hit_near = NULL, hit_far = NULL;

    // Get all entities with mesh components:
    std::vector<ComponentMesh*> meshes = 
        root_entity->GetComponentsInDescendants<ComponentMesh>();

    Entity* best_picking_candidate_entity = nullptr;
    float distance_max = segment.Length();

    for (ComponentMesh* mesh : meshes)
    {
        math::LineSegment segment_local(segment);
        
        segment_local.Transform(mesh->Owner()->Transform()->GetLocalMatrix().Inverted());

        const TriangleArray& triangles = mesh->GetTriangles();

        for (const Triangle& triangle : triangles)
        {
            float distance;
            math::float3 hit_point;

            if (segment_local.Intersects(triangle, &distance, &hit_point))
            {
                if (distance < distance_max)
                {
                    best_picking_candidate_entity = mesh->Owner();
                    distance_max = distance;
                }
            }
            
            LOG("HITPOINT %f, %f, %f", hit_point.x, hit_point.y, hit_point.z);
        }   
    }

    SetSelectedEntity(best_picking_candidate_entity);
}
#include "Scene.h"
#include "Application.h"

#include "ComponentCamera.h"
#include "ComponentLight.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentBoundingBox.h"

#include "ModelImporter.h"

<<<<<<< Updated upstream
#include "MATH_GEO_LIB/Geometry/Triangle.h"
#include "MATH_GEO_LIB/Geometry/LineSegment.h"

=======
>>>>>>> Stashed changes

Scene::Scene() :
    root_entity(nullptr),
    selected_entity(nullptr),
    main_camera(nullptr),
    components_changed_in_descendants_event_listener(components_changed_in_descendants_event_listener),
    hierarchy_changed_event_listener(hierarchy_changed_event_listener)
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

    // Subscribed to hierarchy changed event of the root entity:
    hierarchy_changed_event_listener =
        EventListener<entity_operation>
        (
            std::bind
            (
                &Scene::HandleHierarchyChangedEvent,
                this,
                std::placeholders::_1
            )
        );
    root_entity->GetHierarchyChangedEvent()
        ->AddListener(&hierarchy_changed_event_listener);

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
    camera_entity->Transform()->SetPosition(math::float3(20.0f, 50.0f, -15.0f));
    main_camera->LookAt(math::float3(0.0f, 50.0f, 0.0f));
    
    // Add an entity with a light component as a child of root_entity:
    Entity* light_entity_directional = new Entity();
    light_entity_directional->Initialize("Directional Light");
    light_entity_directional->SetParent(root_entity);
    light_entity_directional->AddComponent<ComponentLight>();
    light_entity_directional->GetComponent<ComponentLight>()->Load(light_type::DIRECTIONAL);

    //Scene
    Entity* light_entity_point = new Entity();
    light_entity_point->Initialize("Point Light");
    light_entity_point->SetParent(root_entity);
    light_entity_point->AddComponent<ComponentLight>();
    light_entity_point->GetComponent<ComponentLight>()->Load(light_type::POINT);
    light_entity_point->Transform()->SetPosition(math::float3(12.0f, 3.0f, -18.0f));
    light_entity_point->GetComponent<ComponentLight>()->SetLightColor(math::float3(1.0f, 0.2f, 0.0f));

    Entity* light_entity_spot = new Entity();
    light_entity_spot->Initialize("Spotlight");
    light_entity_spot->SetParent(root_entity);
    light_entity_spot->AddComponent<ComponentLight>();
    light_entity_spot->GetComponent<ComponentLight>()->Load(light_type::SPOT);
    light_entity_spot->Transform()->SetPosition(math::float3(23.0f, 14.0f, -25.0f));


    std::string working_path = App->GetWorkingDirectory();
    std::string clock_path = working_path + "\\Models\\Clock.fbx";
    Entity* clock_entity = ModelImporter::Import(clock_path.c_str());
    clock_entity->SetParent(root_entity);
    clock_entity->Transform()->SetPosition(math::float3(0.0f, 1.0f, 0.0f));

    std::string doll_path = working_path + "\\Models\\Dollhouse.fbx";
    Entity* doll_entity = ModelImporter::Import(doll_path.c_str());
    doll_entity->SetParent(root_entity);
    doll_entity->Transform()->SetPosition(math::float3(38.0f, 1.0f, -4.0f));

    std::string drawers_path = working_path + "\\Models\\Drawers.fbx";
    Entity* drawers_entity = ModelImporter::Import(drawers_path.c_str());
    drawers_entity->SetParent(root_entity);
    drawers_entity->Transform()->SetPosition(math::float3(8.0f, 1.0f, -4.0f));

    std::string firetruck_path = working_path + "\\Models\\Firetruck.fbx";
    Entity* firetruck_entity = ModelImporter::Import(firetruck_path.c_str());
    firetruck_entity->SetParent(root_entity);
    firetruck_entity->Transform()->SetPosition(math::float3(0.0f, 1.0f, 0.0f));

    std::string floor_path = working_path + "\\Models\\Floor.fbx";
    Entity* floor_entity = ModelImporter::Import(floor_path.c_str());
    floor_entity->SetParent(root_entity);
    floor_entity->Transform()->SetPosition(math::float3(0.0f, 1.0f, 0.0f));

    std::string hearse_path = working_path + "\\Models\\Hearse.FBX";
    Entity* hearse_entity = ModelImporter::Import(hearse_path.c_str());
    hearse_entity->SetParent(root_entity);
    hearse_entity->Transform()->SetPosition(math::float3(12.0f, 1.0f, -18.0f));

    std::string player_path = working_path + "\\Models\\Player.fbx";
    Entity* player_entity = ModelImporter::Import(player_path.c_str());
    player_entity->SetParent(root_entity);
    player_entity->Transform()->SetPosition(math::float3(20.0f, 1.0f, -20.0f));

    std::string robot_path = working_path + "\\Models\\Robot.FBX";
    Entity* robot_entity = ModelImporter::Import(robot_path.c_str());
    robot_entity->SetParent(root_entity);
    robot_entity->Transform()->SetPosition(math::float3(23.0f, 1.0f, -18.0f));

    std::string spinning_path = working_path + "\\Models\\SpinningTop.fbx";
    Entity* spinning_entity = ModelImporter::Import(spinning_path.c_str());
    spinning_entity->SetParent(root_entity);
    spinning_entity->Transform()->SetPosition(math::float3(5.0f, 1.0f, -10.0f));

    std::string wall_path = working_path + "\\Models\\Wall.FBX";
    Entity* wall_entity = ModelImporter::Import(wall_path.c_str());
    wall_entity->SetParent(root_entity);
    wall_entity->Transform()->SetPosition(math::float3(0.0f, 1.0f, 0.0f));

    std::string zombunny_path = working_path + "\\Models\\Zombunny.fbx";
    Entity* zombunny_entity = ModelImporter::Import(zombunny_path.c_str());
    zombunny_entity->SetParent(root_entity);
    zombunny_entity->Transform()->SetPosition(math::float3(9.0f, 1.0f, -20.0f));

    /*
        Entity* player_model = ModelImporter::Import();
        player_model->SetParent(root_entity);
    */


    // Set selected entity as the light_entity:
    // NOTE: If new stuff is added into SetSelectedEntity, 
    // just use that instead of this.
    selected_entity = camera_entity;
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

void Scene::HandleHierarchyChangedEvent(entity_operation operation)
{
    if (operation != entity_operation::CHILDREN_CHANGED)
    {
        return;
    }

    // Get mesh components in scene:
    mesh_components_in_scene.clear();
    mesh_components_in_scene = (root_entity->GetComponentsInDescendants<ComponentMesh>());
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
    // Get all entities with mesh components:
    std::vector<ComponentMesh*> meshes = 
        root_entity->GetComponentsInDescendants<ComponentMesh>();
    Entity* best_picking_candidate_entity = nullptr;

    float distance_max = segment.Length();

    // NOTE: This only works when the camera is inside the model.
    // We couldn't find any fix for now, so it stays like this.

    for (ComponentMesh* mesh : meshes)
    {
        math::LineSegment segment_local(segment);
        
        segment_local.Transform(mesh->Owner()->Transform()->GetMatrix().Inverted());

        const TriangleArray& triangles = mesh->GetTriangles();

        for (const Triangle& triangle : triangles)
        {
            float distance;
            math::float3 hit_point = math::float3::zero;

            if (segment_local.Intersects(triangle, &distance, &hit_point))
            {   
                if (distance < distance_max)
                {
                    best_picking_candidate_entity = mesh->Owner();
                    distance_max = distance;
                }
            }
            
        }   
    }

    if (best_picking_candidate_entity != nullptr)
    {
        SetSelectedEntity(best_picking_candidate_entity);
    }
}
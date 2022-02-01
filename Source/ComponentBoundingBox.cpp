#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "Entity.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "MATH_GEO_LIB/Geometry/AABB.h"
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"
#include "MATH_GEO_LIB/Geometry/Sphere.h"

ComponentBoundingBox::ComponentBoundingBox() : Component(), minimal_enclosing_sphere_radius(10.0f)
{

}

ComponentBoundingBox::~ComponentBoundingBox()
{
    if (owner != nullptr)
    {
        owner->GetComponentsChangedEvent()->RemoveListener(&component_changed_event_listener);
        owner->GetComponentsChangedInDescendantsEvent()->RemoveListener(&component_changed_event_listener);
    }
}

component_type ComponentBoundingBox::Type() const
{
    return component_type::BOUNDING_BOX;
}

void ComponentBoundingBox::Initialize(Entity* new_owner)
{
    Component::Initialize(new_owner);

    Load();
    
    component_changed_event_listener = EventListener<component_type>(std::bind(&ComponentBoundingBox::HandleComponentChanged, this, std::placeholders::_1));
    
    owner->GetComponentsChangedEvent()->AddListener(&component_changed_event_listener);
    owner->GetComponentsChangedInDescendantsEvent()->AddListener(&component_changed_event_listener);
}

void ComponentBoundingBox::Load()
{
    // NOTE: For now we only get mesh components to calculate the bounding box,
    // In the future when colliders are added for example, we may need to include them
    // as well.

    // NOTE: Should this function use already calculated OBBs of lower ComponentBoundingBoxes 
    // when called with GetComponentsChangedInDescendantsEvent which is sure of being notified
    // by it's lower?
    // TODO: Address this issue if it may be a good optimization.

    // Initialize OBB:
    obb.SetNegativeInfinity();

    // Initialize a temp AABB:
    math::AABB aabb;
    aabb.SetNegativeInfinity();

    std::vector<ComponentMesh*> mesh_components = owner->GetComponentsInChildren<ComponentMesh>();

    // TODO(Baran): Refactor this code to remove duplicate code here.

    ComponentMesh* owner_mesh_component = owner->GetComponent<ComponentMesh>();
    if (owner_mesh_component != nullptr && mesh_components.size() > 0)
    {
        mesh_components.push_back(owner_mesh_component);

        for (ComponentMesh* mesh_component : mesh_components)
        {
            const math::AABB& mesh_aabb = mesh_component->GetAABB();
            math::OBB local_obb;

            local_obb.SetFrom(mesh_aabb);
            local_obb.Scale(math::float3::zero, mesh_component->Owner()->Transform()->GetScale());
            local_obb.Transform(mesh_component->Owner()->Transform()->GetRotation());
            local_obb.Translate(mesh_component->Owner()->Transform()->GetPosition());

            aabb.Enclose(local_obb);
        }
    }
    else if (owner->GetComponent<ComponentMesh>() != nullptr && mesh_components.size() <= 0)
    {
        const math::AABB& mesh_aabb = owner_mesh_component->GetAABB();
        math::OBB local_obb;

        obb.SetFrom(mesh_aabb);
        obb.Scale(math::float3::zero, Owner()->Transform()->GetScale());
        obb.Transform(Owner()->Transform()->GetRotation());
        obb.Translate(Owner()->Transform()->GetPosition());
    }
    else if (owner->GetComponent<ComponentMesh>() == nullptr && mesh_components.size() > 0)
    {
        for (ComponentMesh* mesh_component : mesh_components)
        {
            const math::AABB& mesh_aabb = mesh_component->GetAABB();
            math::OBB local_obb;

            local_obb.SetFrom(mesh_aabb);

            local_obb.Scale(math::float3::zero, mesh_component->Owner()->Transform()->GetScale());
            local_obb.Transform(mesh_component->Owner()->Transform()->GetRotation());
            local_obb.Translate(mesh_component->Owner()->Transform()->GetPosition());


            aabb.Enclose(local_obb);
        }
        obb.SetFrom(aabb);
    }
    else
    {
        obb.SetFrom(math::AABB(math::float3(-0.5f, -0.5f, -0.5f), math::float3(0.5f, 0.5f, 0.5f)));
        obb.Transform(Owner()->Transform()->GetRotation());
        obb.Translate(Owner()->Transform()->GetPosition());
    }

    // NOTE: For now this is not a true obb, it's just an obb that acts like aabb,
    // rotations are not truly captured to be more precise. Maybe using child bound
    // ing boxes would give a better result.

    //// Form OBB from temp aabb:
    //obb.SetFrom(aabb);

    // Get Minimal enclosing sphere radius:
    const math::Sphere minimal_enclosing_sphere = obb.MinimalEnclosingSphere();
    minimal_enclosing_sphere_radius = minimal_enclosing_sphere.r;
    center_position = minimal_enclosing_sphere.Centroid();
}

void ComponentBoundingBox::Update()
{
    // NOTE: It does not make any sense to disabling 
    // ComponentBoundingBox stop it's update or other
    // functions. So it's not added here. But Gizmos are not
    // shown like the others.
}

void ComponentBoundingBox::DrawGizmo()
{
    if (!Enabled() || !owner->IsActive())
    {
        return;
    }

    static const int order[8] = { 0, 1, 5, 4, 2, 3, 7, 6 };
    float3 vertices[8];
    for (int i = 0; i < 8; ++i)
    {
        vertices[i] = obb.CornerPoint(order[i]);
    }
        
    App->debug_draw->DrawCuboid(vertices, math::float3(0.0f, 1.0f, 0.0f));
}

const math::OBB& ComponentBoundingBox::GetBoundingBox()
{
    return obb;
}

float ComponentBoundingBox::GetMinimalEnclosingSphereRadius()
{
    return minimal_enclosing_sphere_radius;
}

math::float3 ComponentBoundingBox::GetCenterPosition()
{
    return center_position;
}


void ComponentBoundingBox::HandleComponentChanged(component_type type)
{
    // NOTE: If any component of type other than MESH or TRANSFORM would change
    // the size of the BOUNDING_BOX, make this function aware of them as
    // well.

    switch (type)
    {
        case component_type::TRANSFORM:
        case component_type::MESH:
            break;
        default:
            return;
    }

    Load();
}

void ComponentBoundingBox::DrawInspectorContent()
{
    math::float3 obb_half_size = obb.HalfSize();
    math::float3 obb_position = obb.pos;

    ImGui::TextWrapped("Half Size: %f, %f, %f", obb_half_size.x, obb_half_size.y, obb_half_size.z);
    ImGui::TextWrapped("Position: %f, %f, %f", obb_position.x, obb_position.y, obb_position.z);
}
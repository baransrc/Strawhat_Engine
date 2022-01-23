#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "Entity.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "MATH_GEO_LIB/Geometry/AABB.h"
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"
#include "MATH_GEO_LIB/Geometry/Sphere.h"

ComponentBoundingBox::ComponentBoundingBox() : minimal_enclosing_sphere_radius(10.0f)
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

    std::vector<Component*> mesh_components = owner->GetComponentsIncludingChildren(component_type::MESH);

    for (Component* mesh_component : mesh_components)
    {
        ComponentMesh* mesh = (ComponentMesh*)mesh_component;
        const math::AABB& mesh_aabb = mesh->GetAABB();

        aabb.Enclose(mesh_aabb);
    }

    // Form OBB from temp aabb:
    obb.SetFrom(aabb);
    // Transform obb according to the transform component of owner:
    obb.Transform(owner->Transform()->GetMatrix());

    // Get Minimal enclosing sphere radius:
    const math::Sphere minimal_enclosing_sphere = obb.MinimalEnclosingSphere();
    minimal_enclosing_sphere_radius = minimal_enclosing_sphere.r;
    center_position = minimal_enclosing_sphere.Centroid();
}

void ComponentBoundingBox::Update()
{
}

void ComponentBoundingBox::DrawGizmo()
{
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
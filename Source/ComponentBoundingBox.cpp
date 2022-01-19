#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
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
        owner->GetComponentsChangedEvent()->RemoveListener(&reload_listener);
        owner->GetComponentsChangedInDescendantsEvent()->RemoveListener(&reload_listener);
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
    
    reload_listener = EventListener<component_type>(std::bind(&ComponentBoundingBox::Reload, this, std::placeholders::_1));

    owner->GetComponentsChangedEvent()->AddListener(&reload_listener);
    owner->GetComponentsChangedInDescendantsEvent()->AddListener(&reload_listener);
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
        const math::AABB* mesh_aabb = mesh->GetAABB();

        aabb.Enclose(*mesh_aabb);
    }

    // Form OBB from temp aabb:
    obb.SetFrom(aabb);

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
    App->debug_draw->DrawCuboid(obb.ToPolyhedron().VertexArrayPtr(), math::float3(0.0f, 1.0f, 0.0f));
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


void ComponentBoundingBox::Reload(component_type type)
{
    // NOTE: If any component of type other than MESH would change
    // the size of the BOUNDING_BOX, make this function aware of them as
    // well.

    if (type != component_type::MESH)
    {
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
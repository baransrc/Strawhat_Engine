#include "ComponentBoundingBox.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "MATH_GEO_LIB/Geometry/AABB.h"
#include "MATH_GEO_LIB/Geometry/Polyhedron.h"
#include "MATH_GEO_LIB/Geometry/Sphere.h"

ComponentBoundingBox::ComponentBoundingBox() : 
    Component(), 
    minimal_enclosing_sphere_radius(1.0f)
{
}

ComponentBoundingBox::~ComponentBoundingBox()
{
    if (owner != nullptr)
    {
        // Unsubscribe from the component changed events of the owner:
        owner->GetComponentsChangedEvent()->RemoveListener(&component_changed_event_listener);
        owner->GetComponentsChangedInDescendantsEvent()->RemoveListener(&component_changed_event_listener);
        // Unsubscribe from the hierarchy changed event of the owner:
        owner->GetHierarchyChangedEvent()->RemoveListener(&hierarchy_changed_event_listener);
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

    // Create hierarchy changed event listener:
    hierarchy_changed_event_listener = EventListener<entity_operation>(std::bind(&ComponentBoundingBox::HandleHierarchyChanged, this, std::placeholders::_1));
    // Subscribe it to the owner's get hierarchy changed event:
    owner->GetHierarchyChangedEvent()->AddListener(&hierarchy_changed_event_listener);
    
    // Create component changed event listener:
    component_changed_event_listener = EventListener<component_type>(std::bind(&ComponentBoundingBox::HandleComponentChanged, this, std::placeholders::_1));
    // Subscribe it to the components changed event and
    // the components changed in descendants event of the
    // owner:
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

    std::vector<ComponentMesh*> mesh_components = owner->GetComponentsInDescendants<ComponentMesh>();

    ComponentMesh* owner_mesh_component = owner->GetComponent<ComponentMesh>();

    if (mesh_components.size() > 0)
    {
        // If the owner also has a mesh component, add it to the list:
        if (owner_mesh_component != nullptr)
        {
            mesh_components.push_back(owner_mesh_component);
        }

        // Calculate the aabb from the list:
        for (ComponentMesh* mesh_component : mesh_components)
        {
            AddMeshComponentToAABB(mesh_component, aabb);
        }

        // Set the obb from aabb:
        obb.SetFrom(aabb);
    }
    else if (owner_mesh_component != nullptr && mesh_components.size() <= 0)
    {
        // NOTE: This is not added like the previous if statement 
        // because here we directly get the aabb and set an obb
        // from it and transforming it.

        // Get aabb of owner mesh component:
        const math::AABB& mesh_aabb = owner_mesh_component->GetAABB();

        // Set the obb from owner mesh component's aabb:
        obb.SetFrom(mesh_aabb);
        // Transform it in the order of Scale->Rotate->Translate:
        obb.Scale(math::float3::zero, Owner()->Transform()->GetScale());
        obb.Transform(Owner()->Transform()->GetRotation());
        obb.Translate(Owner()->Transform()->GetPosition());
    }
    else
    {
        // Set a 1 unit cube if there is no mesh component in owner or
        // its descendants:
        obb.SetFrom(math::AABB(math::float3(-0.5f, -0.5f, -0.5f), math::float3(0.5f, 0.5f, 0.5f)));
        obb.Transform(Owner()->Transform()->GetRotation());
        obb.Translate(Owner()->Transform()->GetPosition());
        // NOTE: We don't scale obb by the global scale of its parent since
        // we don't want an empty obb to take more space in the world.
    }

    // NOTE: For now this is not a true obb, it's just an obb that acts like aabb,
    // rotations are not truly captured to be more precise. Maybe using child bound
    // ing boxes would give a better result.

    // Get Minimal enclosing sphere radius:
    const math::Sphere minimal_enclosing_sphere = obb.MinimalEnclosingSphere();
    minimal_enclosing_sphere_radius = minimal_enclosing_sphere.r;
    center_position = minimal_enclosing_sphere.Centroid();
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

const math::OBB& ComponentBoundingBox::GetBoundingBox() const
{
    return obb;
}

float ComponentBoundingBox::GetMinimalEnclosingSphereRadius() const
{
    return minimal_enclosing_sphere_radius;
}

const math::float3& ComponentBoundingBox::GetCenterPosition() const
{
    return center_position;
}

void ComponentBoundingBox::DrawInspectorContent()
{
    math::float3 obb_half_size = obb.HalfSize();
    math::float3 obb_position = obb.pos;

    ImGui::TextWrapped("Half Size: %f, %f, %f", obb_half_size.x, obb_half_size.y, obb_half_size.z);
    ImGui::TextWrapped("Position: %f, %f, %f", obb_position.x, obb_position.y, obb_position.z);
}

void ComponentBoundingBox::AddMeshComponentToAABB(ComponentMesh* mesh, math::AABB& aabb) const
{
    // Get AABB of the mesh:
    const math::AABB& mesh_aabb = mesh->GetAABB();
    // Construct a local_obb that will hold the
    // transformed version of mesh's aabb:
    math::OBB local_obb;

    // Set local_obb to have mesh_aabb:
    local_obb.SetFrom(mesh_aabb);
    // Set its scale:
    local_obb.Scale(math::float3::zero, mesh->Owner()->Transform()->GetScale());
    // Set its rotation:
    local_obb.Transform(mesh->Owner()->Transform()->GetRotation());
    // Set its position:
    local_obb.Translate(mesh->Owner()->Transform()->GetPosition());

    // NOTE: These transforms were made separately to have more 
    // precise transform of the obb as the directly transforming
    // it with the matrix of mesh's owner's transform can have errors.

    // Enclose the local_obb with the aabb:
    aabb.Enclose(local_obb);
}

void ComponentBoundingBox::HandleHierarchyChanged(entity_operation operation)
{
    if (operation != entity_operation::CHILDREN_CHANGED)
    {
        return;
    }

    // NOTE: Right now we only care about the changes in the children
    // entity to recalculate this bounding box. However this is not
    // the most optimal way to do this, we could have a more specific
    // event such as HandleChildWithMeshComponent added type of event
    // but it would be so specific that it could also decrease the
    // performance. Right now invoking this function each time the
    // children are changed makes sense.

    Load();
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
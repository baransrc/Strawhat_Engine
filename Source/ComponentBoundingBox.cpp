#include "ComponentBoundingBox.h"
#include "Entity.h"


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

    LOG("Initialized Component Bounding Box (%u)", Id());
    
    reload_listener = EventListener<component_type>(std::bind(&ComponentBoundingBox::Reload, this, std::placeholders::_1));

    LOG("Subscribed to the Component Changed Events (%u)", Id());

    owner->GetComponentsChangedEvent()->AddListener(&reload_listener);
    owner->GetComponentsChangedInDescendantsEvent()->AddListener(&reload_listener);
}

void ComponentBoundingBox::Load()
{
}

void ComponentBoundingBox::Update()
{
}

void ComponentBoundingBox::DrawGizmo()
{
}

void ComponentBoundingBox::Reload(component_type type)
{
    LOG("Detected new component related change: %s", component_type_to_string(type));

    Load();
}

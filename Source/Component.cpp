#include "Component.h"

Component::Component() : enabled(false), owner(nullptr)
{
}

/// <summary>
/// </summary>
/// <returns>Owner of the component.</returns>
const Entity* Component::Owner() const
{
	return owner;
}

/// <summary>
/// 
/// </summary>
/// <returns>Type of the component.</returns>
component_type Component::Type()
{
	return component_type::UNDEFINED;
}

/// <summary>
/// Initializes the component with it's owner.
/// </summary>
/// <param name="new_owner"></param>
void Component::Initialize(Entity* new_owner)
{
	enabled = true;
	owner = new_owner;
}

/// <summary>
/// Enables the component.
/// </summary>
void Component::Enable()
{
	enabled = true;
}

/// <summary>
/// Disables the component.
/// </summary>
void Component::Disable()
{
	enabled = false;
}

/// <summary>
/// Called on each update of the owner Entity.
/// </summary>
void Component::Update()
{
}

/// Draws Gizmo of the component in editor mode.
/// </summary>
/// <summary>
void Component::DrawGizmo()
{

}

/// <summary>
/// 
/// </summary>
/// <returns>If the component is enabled.</returns>
bool Component::Enabled() const
{
	return enabled;
}
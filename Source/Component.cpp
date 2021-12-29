#include "Component.h"
#include "Entity.h"
#include "imgui.h"
#include "stdio.h"
#include "Globals.h"

Component::Component() : enabled(false), owner(nullptr), id(0)
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
component_type Component::Type() const
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
	id = GetCurrentId();
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

/// <summary>
/// Draws Gizmo of the component in editor mode.
/// </summary>
void Component::DrawGizmo()
{

}

/// <summary>
/// Draws Component related info to Inspector in editor mode.
/// </summary>
void Component::DrawInspector()
{
	char id_buffer[64];

	sprintf_s(id_buffer, 64, "cmp##%u\0", Id());

	ImGui::PushID(id_buffer);
	if (ImGui::CollapsingHeader(component_type_to_string(Type())))
	{
		ImGui::TextWrapped("ID: %u", Id());
		DrawInspectorContent();
	}

	if (ImGui::BeginPopupContextItem(id_buffer))
	{
		if (ImGui::Selectable("Remove Component"))
		{
			owner->RemoveComponent(this);
		}
		ImGui::EndPopup();
	}
	ImGui::PopID();
}

/// <summary>
/// Draws Inspector content of the component in editor mode.
/// Use this function to draw custom content inside Inspector.
/// To draw anything, you must use ImGui widgets, otherwise, you
/// may get weird results.
/// </summary>
void Component::DrawInspectorContent()
{
	ImGui::TextWrapped("You Can override Component::DrawInspectorContent to customize this Inspector content");
}

/// <summary>
/// Used for giving an id to the Component in Initialize method.
/// </summary>
/// <returns>A runtime unique id to the caller</returns>
unsigned int Component::GetCurrentId()
{
	static unsigned int current_id = 0;

	return current_id++;
}

/// <summary>
/// 
/// </summary>
/// <returns>If the component is enabled.</returns>
bool Component::Enabled() const
{
	return enabled;
}

unsigned int Component::Id() const
{
	return id;
}

#include "Component.h"
#include "Entity.h"

#include "Globals.h"

#include "imgui.h"
#include "stdio.h"

Component::Component() : 
	enabled(false), 
	owner(nullptr)
{
	id = GetCurrentId();
}

Entity* Component::Owner() const
{
	return owner;
}

component_type Component::Type() const
{
	return component_type::UNDEFINED;
}

bool Component::CanBeMoreThanOne(component_type type)
{
	// NOTE: If there is a component that can be more than one in an entity,
	// make it's type mapped to true here.

	switch (type)
	{
		case component_type::UNDEFINED:
		{
			return true;
		}
		default:
		{
			return false;
		}
	}
}

void Component::Initialize(Entity* new_owner)
{
	enabled = true;
	owner = new_owner;
	owner->AddComponent(this);
}

void Component::Enable()
{
	enabled = true;
}

void Component::Disable()
{
	enabled = false;
}

void Component::PreUpdate()
{
}


void Component::Update()
{
}


void Component::PostUpdate()
{
}

void Component::DrawGizmo()
{

}

void Component::DrawInspector()
{
	char id_buffer[64];

	sprintf_s(id_buffer, 64, "cmp##%u\0", Id());

	ImGui::PushID(id_buffer);
	if (ImGui::CollapsingHeader(component_type_to_string(Type()), ImGuiTreeNodeFlags_DefaultOpen))
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

void Component::DrawInspectorContent()
{
	ImGui::TextWrapped("You Can override Component::DrawInspectorContent to customize this Inspector content");
}

unsigned int Component::GetCurrentId()
{
	static unsigned int current_id = 0;

	return current_id++;
}

bool Component::Enabled() const
{
	return enabled;
}

unsigned int Component::Id() const
{
	return id;
}
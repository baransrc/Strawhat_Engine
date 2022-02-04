#include "ComponentLight.h"
#include "ComponentTransform.h"

#include "ComponentTransform.h"
#include "Entity.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"

#include "GLEW/include/GL/glew.h"

ComponentLight::ComponentLight() : 
	Component(),
	type(light_type::POINT),
	radius(10),
	inner(10),
	outer(20),
	shininess(1),
	intensity(1),
	color(1,1,1),
	is_currently_loaded(false)
{
}

ComponentLight::~ComponentLight()
{
	Reset();
}

component_type ComponentLight::Type() const
{
	return component_type::LIGHT;
}

void ComponentLight::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);
}

void ComponentLight::Load(light_type new_type)
{
	type = new_type;
	
	color = float3(1, 1, 1);

	radius = 10.0f;
	inner = 10.0f;
	outer = 20.0f;
	shininess = 5.0f;
	intensity = 1.0f;

	switch (type)
	{
		default:
		case light_type::POINT:
		{
			// Make sure we are using the true shader before passing the arguments:
			App->shader_program->Use();

			// Pass variables to the shader
			SetUniformsPointLight();

			break;
		}
		case light_type::DIRECTIONAL:
		{
			// Make sure we are using the true shader before passing the arguments:
			App->shader_program->Use();

			// Pass variables to the shader
			SetUniformsDirectionalLight();

			break;
		}
		case light_type::SPOT:
		{
			// Make sure we are using the true shader before passing the arguments:
			App->shader_program->Use();

			// Pass variables to the shader
			SetUniformsSpotLight();

			break;
		}
	}

	// Set as currently loaded:
	is_currently_loaded = true;
}

void ComponentLight::Update()
{
	if (!Enabled())
	{
    return;
	}

	switch (type)
	{
		default:
		case light_type::POINT:
		{
			// Pass variables to the shader
			SetUniformsPointLight();

			break;
		}
      
    // TODO: MRG.
		case light_type::DIRECTIONAL:
		{
			// Pass variables to the shader
			SetUniformsDirectionalLight();

      break;
		}
		case light_type::SPOT:
		{
			// Pass variables to the shader
			SetUniformsSpotLight();

			//Draw cone
			App->debug_draw->DrawCone(owner->Transform()->GetPosition(), owner->Transform()->GetFront(), radius, inner, float3(0.8f, 0.6f, 1.0f));
			App->debug_draw->DrawCone(owner->Transform()->GetPosition(), owner->Transform()->GetFront(), radius, outer, float3(0.4f, 0.6f, 1.0f));

			break;
		}
	}
}

void ComponentLight::Reset()
{
	is_currently_loaded = false;
}

void ComponentLight::DrawGizmo()
{
	if (!Enabled() || !owner->IsActive())
	{
		return;
	}

	Component::DrawGizmo();
}

void ComponentLight::DrawInspectorContent()
{
	bool enabled_editor = Enabled();
	if (ImGui::Checkbox("Enabled", &enabled_editor))
	{
		enabled_editor ? Enable() : Disable();
	}

	ImGui::NewLine();

	ImGui::Text("Type:");
	if (ImGui::BeginCombo("", component_light_type_to_string(type)))
	{

		if (ImGui::Selectable("Point"))
			type = light_type::POINT;
		if (ImGui::Selectable("Directional"))
			type = light_type::DIRECTIONAL;
		if (ImGui::Selectable("Spot"))
			type = light_type::SPOT;

		ImGui::EndCombo();
	}

	ImGui::PushItemWidth(200.0f);
	ImGui::ColorPicker3("Clear Color", &color[0], ImGuiColorEditFlags_DisplayRGB);
	ImGui::DragFloat("Shininess", &shininess, 0.1f, -inf, inf);
	ImGui::DragFloat("Radius", &radius, 0.1f, -inf, inf);
	ImGui::DragFloat("Inner", &inner, 0.1f, -inf, inf);
	ImGui::DragFloat("Outer", &outer, 0.1f, -inf, inf);
	ImGui::DragFloat("Intensity", &intensity, 0.1f, -inf, inf);

}

void ComponentLight::SetUniformsPointLight()
{
	App->shader_program->SetUniformVariable("lightP.position", owner->Transform()->GetPosition());
	App->shader_program->SetUniformVariable("lightP.radius", radius);
	App->shader_program->SetUniformVariable("lightP.ambient", float3(0.2, 0.2, 0.2));	
	App->shader_program->SetUniformVariable("lightP.diffuse", color);
	App->shader_program->SetUniformVariable("lightP.constant", 1.0f);
	App->shader_program->SetUniformVariable("lightP.linear", 0.9f);
	App->shader_program->SetUniformVariable("lightP.quadratic", 0.032f);

	App->shader_program->SetUniformVariable("lightP.shininess", shininess);
	App->shader_program->SetUniformVariable("lightP.intensity", intensity);
}

void ComponentLight::SetUniformsDirectionalLight()
{
	App->shader_program->SetUniformVariable("lightD.direction", owner->Transform()->GetFront());
	App->shader_program->SetUniformVariable("lightD.ambient", float3(0.2, 0.2, 0.2));
	App->shader_program->SetUniformVariable("lightD.diffuse", color);
	App->shader_program->SetUniformVariable("lightD.intensity", intensity);
	App->shader_program->SetUniformVariable("lightD.shininess", shininess);
}

void ComponentLight::SetUniformsSpotLight()
{
	App->shader_program->SetUniformVariable("lightS.position", owner->Transform()->GetPosition());
	App->shader_program->SetUniformVariable("lightS.direction", owner->Transform()->GetFront());
	App->shader_program->SetUniformVariable("lightS.radius", radius);
	App->shader_program->SetUniformVariable("lightS.inner", inner);
	App->shader_program->SetUniformVariable("lightS.outer", outer);
	App->shader_program->SetUniformVariable("lightS.ambient", float3(0.2, 0.2, 0.2));
	App->shader_program->SetUniformVariable("lightS.diffuse", color);
	App->shader_program->SetUniformVariable("lightS.specular", float3(1.0f, 1.0f, 1.0f));

	App->shader_program->SetUniformVariable("lightS.constant", 1.0f);
	App->shader_program->SetUniformVariable("lightS.linear", 0.9f);
	App->shader_program->SetUniformVariable("lightS.quadratic", 0.032f);
	App->shader_program->SetUniformVariable("lightS.shininess", shininess);
	App->shader_program->SetUniformVariable("lightS.intensity", intensity);
}
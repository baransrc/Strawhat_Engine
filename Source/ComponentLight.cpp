#include "ComponentLight.h"
#include "ComponentTransform.h"

#include "Entity.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "GLEW/include/GL/glew.h"

ComponentLight::ComponentLight() : 
	Component(),
	current_light_type(light_type::POINT),
	radius(10.0f),
	inner_angle(10.0f),
	outer_angle(20.0f),
	intensity(1.0f),
	color(math::float3::one)
{
}

ComponentLight::~ComponentLight()
{
}

component_type ComponentLight::Type() const
{
	return component_type::LIGHT;
}

void ComponentLight::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);
}

void ComponentLight::Update()
{
	if (!Enabled())
	{
		return;
	}

	// Make sure we are using the true shader before passing the arguments:
	App->shader_program->Use();

	// Pass uniforms to the shader:
	SetUniforms();
}

void ComponentLight::DrawGizmo()
{
	if (!Enabled() || !owner->IsActive())
	{
		return;
	}

	// We don't have a gizmo for other light types yet so if it's 
	// not Spot, we don't do anything here and return:
	if (current_light_type != light_type::SPOT)
	{
		return;
	}

	//Draw inner cone:
	App->debug_draw->DrawCone
	(
		owner->Transform()->GetPosition(),
		owner->Transform()->GetFront(),
		radius,
		inner_angle,
		math::float3(0.8f, 0.6f, 1.0f)
	);

	// Draw outer cone:
	App->debug_draw->DrawCone
	(
		owner->Transform()->GetPosition(),
		owner->Transform()->GetFront(),
		radius,
		outer_angle,
		math::float3(0.4f, 0.6f, 1.0f)
	);
}

void ComponentLight::SetLightType(light_type new_type)
{
	current_light_type = new_type;
}

void ComponentLight::SetRadius(float new_radius)
{
	radius = new_radius;
}

void ComponentLight::SetInnerAngle(float new_inner_angle)
{
	inner_angle = new_inner_angle;
}

void ComponentLight::SetOuterAngle(float new_outer_angle)
{
	outer_angle = new_outer_angle;
}

void ComponentLight::SetIntensity(float new_intensity)
{
	intensity = new_intensity;
}

void ComponentLight::SetColor(const math::float3& new_color)
{
	color = new_color;
}

light_type ComponentLight::GetLightType() const
{
	return light_type();
}

const math::float3& ComponentLight::GetColor() const
{
	return color;
}

const float ComponentLight::GetRadius() const
{
	return radius;
}

const float ComponentLight::GetInnerAngle() const
{
	return inner_angle;
}

const float ComponentLight::GetOuterAngle() const
{
	return outer_angle;
}

const float ComponentLight::GetIntensity() const
{
	return intensity;
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
	
	if (ImGui::BeginCombo("", component_light_type_to_string(current_light_type)))
	{

		if (ImGui::Selectable("Point"))
			current_light_type = light_type::POINT;
		if (ImGui::Selectable("Directional"))
			current_light_type = light_type::DIRECTIONAL;
		if (ImGui::Selectable("Spot"))
			current_light_type = light_type::SPOT;

		ImGui::EndCombo();
	}

	ImGui::PushItemWidth(200.0f);
	
	ImGui::ColorPicker3("Color", &color[0], ImGuiColorEditFlags_DisplayRGB);
	
	ImGui::DragFloat("Intensity", &intensity, 0.1f, -inf, inf);

	if (current_light_type == light_type::POINT)
	{
		ImGui::DragFloat("Radius", &radius, 0.1f, -inf, inf);
	}

	if (current_light_type == light_type::SPOT)
	{
		ImGui::DragFloat("Inner", &inner_angle, 0.1f, -inf, inf);
		ImGui::DragFloat("Outer", &outer_angle, 0.1f, -inf, inf);
	}

	ImGui::PopItemWidth();
}

void ComponentLight::SetUniforms()
{
	switch (current_light_type)
	{
		default:
		case light_type::POINT:
		{
			SetUniformsPointLight();
		}
		break;

		case light_type::DIRECTIONAL:
		{
			SetUniformsDirectionalLight();
		}
		break;

		case light_type::SPOT:
		{
			SetUniformsSpotLight();
		}
		break;
	}
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
	App->shader_program->SetUniformVariable("lightP.intensity", intensity);
}

void ComponentLight::SetUniformsDirectionalLight()
{
	App->shader_program->SetUniformVariable("lightD.direction", owner->Transform()->GetFront());
	App->shader_program->SetUniformVariable("lightD.ambient", float3(0.2, 0.2, 0.2));
	App->shader_program->SetUniformVariable("lightD.diffuse", color);
	App->shader_program->SetUniformVariable("lightD.intensity", intensity);
}

void ComponentLight::SetUniformsSpotLight()
{
	App->shader_program->SetUniformVariable("lightS.position", owner->Transform()->GetPosition());
	App->shader_program->SetUniformVariable("lightS.direction", owner->Transform()->GetFront());
	App->shader_program->SetUniformVariable("lightS.radius", radius);
	App->shader_program->SetUniformVariable("lightS.inner", inner_angle);
	App->shader_program->SetUniformVariable("lightS.outer", outer_angle);
	App->shader_program->SetUniformVariable("lightS.ambient", float3(0.2, 0.2, 0.2));
	App->shader_program->SetUniformVariable("lightS.diffuse", color);
	App->shader_program->SetUniformVariable("lightS.specular", float3(1.0f, 1.0f, 1.0f));
	App->shader_program->SetUniformVariable("lightS.constant", 1.0f);
	App->shader_program->SetUniformVariable("lightS.linear", 0.9f);
	App->shader_program->SetUniformVariable("lightS.quadratic", 0.032f);
	App->shader_program->SetUniformVariable("lightS.intensity", intensity);
}
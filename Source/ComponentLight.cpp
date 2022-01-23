#include "ComponentLight.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleCamera.h"

#include "GLEW/include/GL/glew.h"

ComponentLight::ComponentLight() : Component(),
												type(light_type::SPOT),
												position(0,0,0),
												//rotation(0,0,0,0),
												scale(1,1,1),
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

void ComponentLight::Load(light_type new_type, float3 new_position, /*Quat new_rotation,*/ float3 new_scale, float3 new_color)
{
	type = new_type;
	position = new_position;
	//rotation = new_rotation;
	scale = new_scale;
	color = new_color;

	// If Load was called before this call, clear 
	// all the previous mesh data and load afterwards:
	//if (is_currently_loaded)
	//{
	//	Reset();
	//}


	switch (type)
	{
	case light_type::POINT:
	{

		// Make sure we are using the true shader before passing the arguments:
		App->shader_program->Use();

		// Pass position of the directional light
		float3 aux_pos = App->camera->GetPosition();
		App->shader_program->SetUniformVariable("light.position", position);
		App->shader_program->SetUniformVariable("light.ambient", float3(0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", color);

		break;
	}
	case light_type::DIRECTIONAL:
	{
		// Make sure we are using the true shader before passing the arguments:
		App->shader_program->Use();

		// Pass position of the directional light
		float3 aux_pos = App->camera->GetPosition();
		App->shader_program->SetUniformVariable("light.direction", position);
		App->shader_program->SetUniformVariable("light.ambient", float3(0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", color);

		break;
	}
	case light_type::SPOT:
	{
		// Make sure we are using the true shader before passing the arguments:
		App->shader_program->Use();

		// Pass position of the directional light

		float3 aux_pos = App->camera->GetPosition();
		LOG("Output camera getPosition: %f, %f, %f\n", aux_pos.x, aux_pos.y, aux_pos.z);
		App->shader_program->SetUniformVariable("light.position", aux_pos);
		LOG("Output camera getPosition: %f, %f, %f\n", App->camera->GetPosition().x, App->camera->GetPosition().y, App->camera->GetPosition().z);
		LOG("Output camera getFront: %f, %f, %f\n", App->camera->GetFront().x, App->camera->GetFront().y, App->camera->GetFront().z);
		float3 aux_front = App->camera->GetFront();
		App->shader_program->SetUniformVariable("light.direction", aux_front);
		float aux_angle = math::Cos(math::DegToRad(12.5f));
		App->shader_program->SetUniformVariable("light.cutOff", aux_angle);
		App->shader_program->SetUniformVariable("light.ambient", float3(0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", float3(0.8f, 0.8f, 0.8f));
		App->shader_program->SetUniformVariable("light.specular", float3(1.0f, 1.0f, 1.0f));
		App->shader_program->SetUniformVariable("light.constant", 1.0f);
		App->shader_program->SetUniformVariable("light.linear", 0.9f);
		App->shader_program->SetUniformVariable("light.quadratic", 0.032f);

		break;
	}
	default:
	{
		LOG("The light type is crashing bro");
		break;
	}
	}


	// Set as currently loaded:
	is_currently_loaded = true;
}

void ComponentLight::Update()
{
	switch (type)
	{
	case light_type::POINT:
	{
		// Pass position of the point light
		App->shader_program->SetUniformVariable("light.position", position);
		App->shader_program->SetUniformVariable("light.ambient", float3(0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", color);

		break;
	}
	case light_type::DIRECTIONAL:
	{
		// Make sure we are using the true shader before passing the arguments:
		//App->shader_program->Use();

		// Pass position of the directional light
		App->shader_program->SetUniformVariable("light.direction", position);
		App->shader_program->SetUniformVariable("light.ambient", float3(0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", color);

		break;
	}
	case light_type::SPOT:
	{
		// Make sure we are using the true shader before passing the arguments:
		//App->shader_program->Use();

		// Pass position of the directional light
		App->shader_program->SetUniformVariable("light.position", App->camera->GetPosition());
		App->shader_program->SetUniformVariable("light.direction", App->camera->GetFront());
		App->shader_program->SetUniformVariable("light.cutOff", math::Cos(math::DegToRad(12.5f)));
		App->shader_program->SetUniformVariable("light.ambient", (0.2, 0.2, 0.2));
		App->shader_program->SetUniformVariable("light.diffuse", (0.8f, 0.8f, 0.8f));
		App->shader_program->SetUniformVariable("light.specular", (1.0f, 1.0f, 1.0f));
		App->shader_program->SetUniformVariable("light.constant", 1.0f);
		App->shader_program->SetUniformVariable("light.linear", 0.9f);
		App->shader_program->SetUniformVariable("light.quadratic", 0.032f);

		break;
	}
	default:
	{
		LOG("The light type is crashing bro");
		break;
	}
	}
}

void ComponentLight::Reset()
{
	is_currently_loaded = false;

	/*free(indices);
	free(vertices);
	free(texture_ids);

	indices = nullptr;
	vertices = nullptr;*/

}

void ComponentLight::DrawGizmo()
{
	Component::DrawGizmo();
}

void ComponentLight::DrawInspectorContent()
{
}

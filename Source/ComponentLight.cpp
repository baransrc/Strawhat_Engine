#include "ComponentLight.h"

#include "Application.h"
#include "ModuleShaderProgram.h"

#include "GLEW/include/GL/glew.h"

ComponentLight::ComponentLight() : Component(),
												type(light_type::DIRECTIONAL),
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
		break;
	}
	case light_type::DIRECTIONAL:
	{
		// Make sure we are using the true shader before passing the arguments:
		App->shader_program->Use();

		// Pass position of the directional light
		App->shader_program->SetUniformVariable("directional_light", position);
		App->shader_program->SetUniformVariable("directional_color", color);

		break;
	}
	case light_type::SPOT:
	{
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
	App->shader_program->SetUniformVariable("directional_light", position);
	App->shader_program->SetUniformVariable("directional_color", color);
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

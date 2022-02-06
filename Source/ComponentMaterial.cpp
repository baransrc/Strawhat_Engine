#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "GLEW/include/GL/glew.h"

#include "Entity.h"

// TODO: Add API's for setting shininess or diffuse color of this
// material. They can only be set from the editor for now.

ComponentMaterial::ComponentMaterial() : 
	Component(),
	texture_ids(nullptr),
	number_of_texture_ids(0),
	color_diffuse(math::float4::one),
	shininess(0),
	is_currently_loaded(false)
{
}

ComponentMaterial::~ComponentMaterial()
{
	Reset();
}

component_type ComponentMaterial::Type() const
{
	return component_type::MATERIAL;
}

void ComponentMaterial::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);
}

void ComponentMaterial::Load(const unsigned int* new_texture_ids, size_t new_number_of_texture_ids)
{
	// TODO(Monica): Hardcoded for this assignment, fix this.
	number_of_texture_ids = 3;

	if (is_currently_loaded)
	{
		Reset();
	}

	if (new_texture_ids == nullptr)
	{
		return;
	}

	texture_ids = (unsigned int*)malloc(sizeof(unsigned int) * number_of_texture_ids);

	for (size_t i = 0; i < number_of_texture_ids; ++i)
	{
		texture_ids[i] = new_texture_ids[i];
	}

	// Use the shader:
	App->shader_program->Use();
	// Activate Texture Unit 0:
	glActiveTexture(GL_TEXTURE0);
	// Bind Texture Unit 0:
	glBindTexture(GL_TEXTURE_2D, texture_ids[0]); // Diffuse texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.diffuse", 0);

	// Activate Texture Unit 1:
	glActiveTexture(GL_TEXTURE1);
	// Bind Texture Unit 1:
	glBindTexture(GL_TEXTURE_2D, texture_ids[1]); // Specular texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.specular", 1);

	// Activate Texture Unit 2:
	glActiveTexture(GL_TEXTURE2);
	// Bind Texture Unit 2:
	glBindTexture(GL_TEXTURE_2D, texture_ids[2]); // Occlusion texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.occlusion", 2);

	// Set as currently loaded:
	is_currently_loaded = true;

	// Invoke change in parent and its ancestors:
	owner->InvokeComponentsChangedEvents(Type());
}

void ComponentMaterial::Use()
{
	// Use the shader :
	App->shader_program->Use();
	// Activate Texture Unit 0:
	glActiveTexture(GL_TEXTURE0);
	// Bind Texture Unit 0:
	glBindTexture(GL_TEXTURE_2D, texture_ids[0]); // Diffuse texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.diffuse", 0);

	// Activate Texture Unit 1:
	glActiveTexture(GL_TEXTURE1);
	// Bind Texture Unit 1:
	glBindTexture(GL_TEXTURE_2D, texture_ids[1]); // Specular texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.specular", 1);

	// Activate Texture Unit 2:
	glActiveTexture(GL_TEXTURE2);
	// Bind Texture Unit 2:
	glBindTexture(GL_TEXTURE_2D, texture_ids[2]); // Occlusion texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.occlusion", 2);

	// Set shininess parameter in shader:
	// TODO (Monica): Create a shininess uniform in shader and make all
	// lights use that for this material.
	App->shader_program->SetUniformVariable("lightP.shininess", shininess);
	App->shader_program->SetUniformVariable("lightD.shininess", shininess);
	App->shader_program->SetUniformVariable("lightS.shininess", shininess);
}

void ComponentMaterial::Reset()
{
	is_currently_loaded = false;

	shininess = 0.0f;

	color_diffuse = math::float4::one;

	free(texture_ids);
}

void ComponentMaterial::DrawInspectorContent()
{
	if (texture_ids == nullptr)
	{
		return;
	}

	ImGui::DragFloat("Shininess", &shininess, 0.1f, -inf, inf);

	for (size_t i = 0; i < number_of_texture_ids; ++i)
	{
		if ((intptr_t)texture_ids[i] > 0)
		{
			ImGui::Text("Texture ID: %u", texture_ids[i]);
			ImGui::Image((void*)(intptr_t)texture_ids[i], ImVec2(150, 150));
		}
	}
}
#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleShaderProgram.h"
#include "ModuleDebugDraw.h"

#include "GLEW/include/GL/glew.h"

#include "Entity.h"

ComponentMaterial::ComponentMaterial() : Component(),
										texture_ids(nullptr),
										number_of_texture_ids(0),
										color_diffuse(0.2f, 0.2f, 0.2f, 1.0f),
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
	//number_of_texture_ids = new_number_of_texture_ids;
	number_of_texture_ids = 3;

	texture_ids = (unsigned int*)malloc(sizeof(unsigned int) * number_of_texture_ids);
	for (size_t i = 0; i < number_of_texture_ids; ++i)
	{
		texture_ids[i] = new_texture_ids[i];
		//LOG("Texture %d id %d", i, texture_ids[i]);
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

	// Activate Texture Unit 3:
	glActiveTexture(GL_TEXTURE3);
	// Bind Texture Unit 3:
	glBindTexture(GL_TEXTURE_2D, texture_ids[3]); // NormalMap texture
	// Set Texture Parameter in shader:
	App->shader_program->SetUniformVariable("material.normal", 3);

	// Set as currently loaded:
	is_currently_loaded = true;

	// Invoke change in parent and its ancestors:
	owner->InvokeComponentsChangedEvents(Type());
}

void ComponentMaterial::Update()
{
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
}

void ComponentMaterial::Reset()
{
	is_currently_loaded = false;
}


void ComponentMaterial::DrawInspectorContent()
{
}

#pragma once

#include "Component.h"

#include "MATH_GEO_LIB/Math/float4.h"

class ComponentMaterial : public Component
{
private:
	/// <summary>
	/// Texture ids of this ComponentMaterial.
	///	diffuse -> 0
	///	specular -> 1
	///	oclussion -> 2
	///	normalmap -> 3
	/// </summary>
	unsigned int* texture_ids;

	/// <summary>
	/// Number of texture id's given to this ComponentMaterial.
	/// </summary>
	size_t number_of_texture_ids;

	/// <summary>
	/// Diffuse color of this ComponentMaterial.
	/// </summary>
	math::float4 color_diffuse;

	/// <summary>
	/// Shininess of this ComponentMaterial.
	/// </summary>
	float shininess;

	/// <summary>
	/// Flag that is set to true when this ComponentMaterial is
	/// Loaded, false to when it's Reset or newly created.
	/// </summary>
	bool is_currently_loaded;

public:
	ComponentMaterial();
	~ComponentMaterial() override;

	/// <returns> 
	/// Type of this Component, which is MATERIAL.
	/// </returns>
	component_type Type() const override;

	/// <summary>
	/// Initializes this ComponentMaterial along with it's owner Entity.
	/// </summary>
	/// <param name="new_owner">Entity to be set as the owner.</param>
	void Initialize(Entity* new_owner) override;

	/// <summary>
	/// Loads this ComponentMaterial from given textures.
	/// Also sets is_currently_loaded flag to true.
	/// </summary>
	/// <param name="new_texture_ids">OpenGL Ids of textures.</param>
	/// <param name="new_number_of_texture_ids">Number of given textures.</param>
	void Load
	(
		const unsigned int* new_texture_ids, 
		size_t new_number_of_texture_ids
	);

	/// <summary>
	/// Binds the textures and sets the uniforms related to this ComponentMaterial.
	/// </summary>
	void Use();

	/// <summary>
	/// Resets this ComponentMaterial like it has never been Initialized before.
	/// Also sets the is_currently_loaded flag to false.
	/// </summary>
	void Reset();

protected:
	/// <summary>
	/// Called inside Component::DrawInspector.
	/// </summary>
	void DrawInspectorContent() override;
};
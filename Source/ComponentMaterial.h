#pragma once
#include "Component.h"

#include "MATH_GEO_LIB/Math/float4.h"

class ComponentMaterial : public Component
{
private:
	unsigned int* texture_ids;
	//diffuse 0
	//specular 1
	//oclussion 2
	//normalmap 3

	size_t number_of_texture_ids;

	float4 color_diffuse;

	float shininess;

	bool is_currently_loaded;

public:
	ComponentMaterial();
	~ComponentMaterial() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load(
		const unsigned int* new_texture_ids, 
		size_t new_number_of_texture_ids
	);
	void Update() override;
	void Use();
	void Reset();

protected:
	void DrawInspectorContent() override;

};


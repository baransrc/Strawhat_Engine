#pragma once
#include "Component.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/Quat.h"


enum class light_type
{
	SPOT,
	DIRECTIONAL,
	POINT,
};

class ComponentLight : public Component
{
private:

	light_type type;

	float3 position;
	Quat rotation;
	float3 scale;

	float radius;
	float shininess;
	float intensity;

	float3 color;

	bool is_currently_loaded;

public:
	ComponentLight();
	~ComponentLight() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load(
		light_type new_type, 
		float3 new_position, 
		Quat new_rotation, 
		float3 new_scale, 
		float3 new_color
	);
	void Update() override;
	void Reset();
	void DrawGizmo() override;

	light_type GetLightType() const { return type; };
	float3 GetLightPosition() const { return position; };
	//Quat GetLightRotation() const { return rotation; };
	float3 GetLightScale() const { return scale; };
	float3 GetLightColor() const { return color; };

protected:
	void DrawInspectorContent() override;
};

inline const char* component_light_type_to_string(light_type type)
{
	switch (type)
	{
	case light_type::SPOT:
	{
		return "Spot";
	}
	case light_type::DIRECTIONAL:
	{
		return "Directrional";
	}
	case light_type::POINT:
	{
		return "Point";
	}
	default:
	{
		return "Refer to ComponentLight.h and add this component to the function.";
	}
	}
}


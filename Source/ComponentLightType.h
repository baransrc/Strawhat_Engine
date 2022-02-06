#pragma once

enum class light_type
{
	SPOT,
	DIRECTIONAL,
	POINT,
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
			return "Directional";
		}

		case light_type::POINT:
		{
			return "Point";
		}

		default:
		{
			return "Refer to ComponentLightType.h and add this type to the function.";
		}
	}
}
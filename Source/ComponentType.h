#pragma once

enum class component_type
{
	UNDEFINED,
	TRANSFORM,
	CAMERA,
	MESH,
};

inline const char* component_type_to_string(component_type type)
{
	switch (type)
	{
	case component_type::UNDEFINED:
	{
		return "Undefined Component";
	}
	case component_type::TRANSFORM:
	{
		return "Transform Component";
	}
	case component_type::CAMERA:
	{
		return "Camera Component";
	}
	case component_type::MESH:
	{
		return "Mesh Component";
	}
	default:
	{
		return "Refer to ComponentType.h and add this component to the function.";
	}
	}
}
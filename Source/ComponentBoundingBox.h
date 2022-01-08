#pragma once

#include "Component.h"
#include "Event.h"
#include <functional>

#include "MATH_GEO_LIB/Geometry/OBB.h"

class ComponentBoundingBox : public Component
{
private:
	math::OBB obb;
	float minimal_enclosing_sphere_radius;
	EventListener<component_type> reload_listener;

public:
	ComponentBoundingBox();
	~ComponentBoundingBox() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load();
	void Reload(component_type type);
	void Update() override;
	void DrawGizmo() override;
};


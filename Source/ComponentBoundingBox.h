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
	math::float3 center_position;
	EventListener<component_type> component_changed_event_listener;

public:
	ComponentBoundingBox();
	~ComponentBoundingBox() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load();
	void HandleComponentChanged(component_type type);
	void Update() override;
	void DrawGizmo() override;
	const math::OBB& GetBoundingBox();
	float GetMinimalEnclosingSphereRadius();
	math::float3 GetCenterPosition();

protected:
	void DrawInspectorContent() override;
};
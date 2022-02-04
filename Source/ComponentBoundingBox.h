#pragma once

#include "Entity.h"
#include "Component.h"
#include "Event.h"

#include "MATH_GEO_LIB/Geometry/OBB.h"

class ComponentMesh;

class ComponentBoundingBox : public Component
{
private:
	math::OBB obb;
	float minimal_enclosing_sphere_radius;
	math::float3 center_position;
	EventListener<component_type> component_changed_event_listener;
	EventListener<entity_operation> hierarchy_changed_event_listener;

public:
	ComponentBoundingBox();
	~ComponentBoundingBox() override;

	component_type Type() const override;
	void Initialize(Entity* new_owner) override;
	void Load();
	void Update() override;
	void DrawGizmo() override;
	const math::OBB& GetBoundingBox();
	float GetMinimalEnclosingSphereRadius();
	math::float3 GetCenterPosition();

protected:
	void DrawInspectorContent() override;

private:
	void AddMeshComponentToAABB(ComponentMesh* mesh, math::AABB& aabb) const;
	void HandleHierarchyChanged(entity_operation operation);
	void HandleComponentChanged(component_type type);
};
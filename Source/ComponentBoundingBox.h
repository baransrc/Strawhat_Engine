#pragma once

#include "Entity.h"
#include "Component.h"
#include "Event.h"

#include "MATH_GEO_LIB/Geometry/OBB.h"

class ComponentMesh;

class ComponentBoundingBox : public Component
{
private:
	/// <summary>
	/// OBB that surrounds the owner Entity.
	/// </summary>
	math::OBB obb;

	/// <summary>
	/// Minimal enclosing sphere radius of the OBB.
	/// </summary>
	float minimal_enclosing_sphere_radius;

	/// <summary>
	/// Center Position of OBB.
	/// </summary>
	math::float3 center_position;

	/// <summary>
	/// Listener to the component changed event of owner Entity.
	/// </summary>
	EventListener<component_type> component_changed_event_listener;

	/// <summary>
	/// Listener to the hierarchy changed event of owner Entity.
	/// </summary>
	EventListener<entity_operation> hierarchy_changed_event_listener;

public:
	ComponentBoundingBox();
	~ComponentBoundingBox() override;

	/// <returns>
	/// Type of this Component.
	/// </returns>
	component_type Type() const override;
	
	/// <summary>
	/// Initializes this ComponentBoundingBox and sets it's parent.
	/// After the Component::Initialize is called, this method calls
	/// Load method and subscribes to the events of it's owner Entity.
	/// </summary>
	/// <param name="new_owner">Entity to be set as owner.</param>
	void Initialize(Entity* new_owner) override;

	/// <summary>
	/// Loads the internal OBB of this ComponentBoundingBox
	/// by traversing all the ComponentMeshes of the hierarchy
	/// of owner including itself.
	/// </summary>
	void Load();
	
	/// <summary>
	/// Draws the OBB. Called on each Entity::DrawGizmos 
	/// of owner entity.
	/// </summary>
	void DrawGizmo() override;

	/// <returns>
	/// A const reference to OBB.
	///</returns>
	const math::OBB& GetBoundingBox() const;

	/// <returns>
	///	Minimal enclosing sphere radius of OBB.
	/// </returns>
	float GetMinimalEnclosingSphereRadius() const;
	
	/// <returns>
	///	Center Position of OBB.
	/// </returns>
	const math::float3& GetCenterPosition() const;

protected:
	/// <summary>
	/// Draws the ImGui content of this.
	/// Called by Component::DrawInspector
	/// </summary>
	void DrawInspectorContent() override;

private:
	/// <summary>
	/// Gets the AABB of mesh, transforms it according to
	/// it's owner's Transform, and adds it to aabb.
	/// </summary>
	/// <param name="mesh">ComponentMesh to be added to aabb.</param>
	/// <param name="aabb">AABB to enclose mesh.</param>
	void AddMeshComponentToAABB(ComponentMesh* mesh, math::AABB& aabb) const;

	/// <summary>
	/// Called each time hierarchy_changed_event_listener's subscribed
	/// event is invoked.
	/// </summary>
	/// <param name="operation">Operation made that causes a change in hierarchy.</param>
	void HandleHierarchyChanged(entity_operation operation);

	/// <summary>
	/// Called each time component_changed_event_listener's subscribed
	/// event is invoked.
	/// </summary>
	/// <param name="type">Type of component that has changed.</param>
	void HandleComponentChanged(component_type type);
};
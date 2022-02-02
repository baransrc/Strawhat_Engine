#pragma once

#include "Component.h"
#include "Event.h"

#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Geometry/OBB.h"
#include "MATH_GEO_LIB/Geometry/LineSegment.h"

enum class camera_projection_mode
{
	PERSPECTIVE,
	ORTHOGRAPHIC,
};

class ComponentCamera : public Component
{
private:
	math::Frustum frustum;
	math::float4x4 projection_matrix;
	math::float4x4 view_matrix;
	camera_projection_mode projection_mode;
	bool should_calculate_projection_matrix;
	bool is_main_camera;
	bool should_render;
	EventListener<component_type> component_changed_event_listener;

public:
	ComponentCamera();
	~ComponentCamera() override;

	void Initialize(Entity* new_owner) override;
	void PreUpdate() override;
	void Update() override;
	void DrawGizmo() override;

	component_type Type() const override;

	const math::float4x4& GetViewMatrix() const;
	const math::float4x4& GetProjectionMatrix() const;
	float GetVerticalFOV() const;
	float GetHorizontalFOV() const;
	float GetAspectRatio() const;
	float GetOrthographicWidth() const;
	float GetOrthographicHeight() const;
	float GetFarPlaneDistance() const;
	float GetNearPlaneDistance() const;
	camera_projection_mode GetProjectionMode() const;
	bool GetShouldRender() const;

	void SetHorizontalFOV(float new_horizontal_fov);
	void SetAspectRatio(float new_aspect_ratio);
	void SetFarPlaneDistance(float new_far_plane_distance);
	void SetNearPlaneDistance(float new_near_plane_distance);
	void SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance);
	void SetIsMainCamera(bool new_is_main_camera);
	void SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio);
	void SetAsOrthographic(float new_orthographic_width, float new_orthographic_height);
	void SetShouldRender(bool new_should_render);

	void LookAt(const math::float3& direction);
	bool DoesOBBHavePointInsideFrustum(const math::OBB& obb) const;

	math::LineSegment GenerateRayFromNormalizedPositions(float x, float y);

protected:
	void DrawInspectorContent() override;

private:
	void CalculateProjectionMatrix();
	void UpdateTransformVariables();
	void HandleComponentChanged(component_type type);
};
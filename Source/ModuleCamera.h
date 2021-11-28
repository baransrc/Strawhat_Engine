#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"

class ModuleCamera : public Module
{
private:
	Frustum frustum;

	// Matrices:
	float4x4 model_matrix;
	float4x4 view_matrix;
	float4x4 projection_matrix;

	float3 direction = float3::zero;
	// Euler Angles:
	float yaw = -90.0f;
	float pitch = 0.0f;

	// Properties:
	bool is_perspective = true;
	bool should_auto_rotate_around_target = false;
	float3 target_position;
	float3 movement_speed;
	float sensitivity = 10.f;
	float orbit_speed;


	// Used as a flag to recalculate the projection matrix in the next PreUpdate:
	bool should_recalculate_projection_matrix = false;

public:
	ModuleCamera();
	~ModuleCamera() override;
	
	bool Init();
	bool CleanUp();

	float4x4 GetModelMatrix() const { return model_matrix; };
	float4x4 GetViewMatrix() const { return view_matrix; };
	float4x4 GetProjectionMatrix() const { return projection_matrix; };
	float3 GetPosition() const { return frustum.Pos(); };
	float GetAspectRatio() const { return frustum.AspectRatio(); };
	
	void SetHorizontalFOV(float new_horizontal_fov);
	void SetAspectRatio(float new_aspect_ratio);
	void SetFarPlaneDistance(float new_far_plane_distance);
	void SetNearPlaneDistance(float new_near_plane_distance);
	void SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance);
	void SetPosition(float3 new_position);
	void SetOrientation(float3 new_orientation);
	void SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio);
	void SetAsOrthographic(float new_orthographic_width, float new_orthographic_height);

	void LookAt(float3 look_at_position);
	void AutoRotateAround(float3 position);

	void WindowResized(unsigned int width, unsigned int height);

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

private:
	void CalculateProjectionMatrix();
	void Move();
	void Rotate();
};
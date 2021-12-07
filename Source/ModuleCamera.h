#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"

enum class vector_mode
{
	DIRECTION,
	POSITION
};

enum class camera_state
{
	FOCUSED,
	FOCUSING,
	UNFOCUSED,
};

class ModuleCamera : public Module
{
private:
	Frustum frustum;

	// Main Matrices:
	float4x4 model_matrix;
	float4x4 view_matrix;
	float4x4 projection_matrix;

	// Matrices that construct view_matrix:
	float4x4 rotation_matrix;
	float4x4 translation_matrix;

	// Current state of camera:
	camera_state state;
	
	// Properties:
	bool locked = true;
	bool is_perspective = true;
	bool should_auto_rotate_around_target = false;
	float sensitivity = 10.f;
	float orbit_speed;
	float zoom_speed;
	float zoom_velocity;
	float zoom_drag;
	float3 movement_speed;
	float3 rotation_euler;

	float3 focus_target_position;
	float3 focus_target_direction;
	float focus_duration;
	float focus_lerp_position;
	float focus_target_radius;

	// Used as a flag to recalculate the projection matrix in the next PreUpdate:
	bool should_recalculate_projection_matrix = false;

public:
	ModuleCamera();
	~ModuleCamera() override;
	
	bool Init();
	bool CleanUp();

	float GetAspectRatio() const { return frustum.AspectRatio(); };
	float4x4 GetModelMatrix() const { return model_matrix; };
	float4x4 GetViewMatrix() const { return view_matrix; };
	float4x4 GetProjectionMatrix() const { return projection_matrix; };
	float3 GetPosition() const;
	float3 GetRotation() const;
	float3 GetUp() const;
	float3 GetFront() const;
	float3 GetDirection() const;
	float3 GetRight() const;

	void SetUp(float3 new_up);
	void SetFront(float3 new_front);
	void SetDirection(float3 new_direction);
	void SetRight(float3 new_right);
	void SetHorizontalFOV(float new_horizontal_fov);
	void SetAspectRatio(float new_aspect_ratio);
	void SetFarPlaneDistance(float new_far_plane_distance);
	void SetNearPlaneDistance(float new_near_plane_distance);
	void SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance);
	void SetPosition(float3 new_position);
	void SetRotation(float3 new_orientation);
	void SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio);
	void SetAsOrthographic(float new_orthographic_width, float new_orthographic_height);

	void LookAt(float3 look_at, vector_mode interpret_as = vector_mode::POSITION, bool calculate_rotation = true);
	void ComputeViewMatrix();
	void AutoRotateAround(float3 position);
	void WindowResized(unsigned int width, unsigned int height);

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

private:
	void CalculateRotationFromDirection();
	void CalculateProjectionMatrix();
	void Move();
	void Rotate();
	void Zoom();
	void ToggleLock();
	void Focus();
	void DetectFocus();
	void ExecuteFocus();
	void SetupFocus(float3 position, float3 size);
	void SetupFocus(float3 position, float bounding_sphere_radius);
};
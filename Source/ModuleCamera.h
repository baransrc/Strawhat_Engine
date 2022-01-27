#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/Quat.h"
#include "Event.h"

class Entity;

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

enum class camera_mouse_input_state
{
	ROTATE,
	ORBIT,
	IDLE,
};

class Entity;
class ComponentCamera;
class ComponentTransform;

class ModuleCamera : public Module
{
private:
	Entity* camera_entity;
	ComponentCamera* camera;
	ComponentTransform* transform;

	//Frustum frustum;

	//// Main Matrices:
	//float4x4 projection_matrix;

	// Current state of camera:
	camera_state state;
	camera_mouse_input_state mouse_input_state;

	// Properties:
	float sensitivity = 10.f;
	float orbit_speed;
	float zoom_speed;
	float zoom_velocity;
	float zoom_drag;
	float3 movement_speed;
	float3 fast_movement_speed;
	float3 rotation_euler;

	bool focus_on_model_changed;
	float3 focus_start_position;
	float3 focus_start_direction;
	math::Quat focus_start_orientation;
	math::Quat focus_target_orientation;
	float3 focus_target_position;
	float3 focus_destination_position;
	float3 focus_target_direction;
	float focus_duration;
	float focus_lerp_position;
	float focus_target_radius;

	// Event Listeners:
	EventListener<unsigned int, unsigned int> window_resized_event_listener;

public:
	ModuleCamera();
	~ModuleCamera() override;
	
	bool Init() override;
	bool CleanUp() override;

	ComponentCamera* const GetCamera() const;
	ComponentTransform* const GetTransform() const;

	void HandleWindowResized(unsigned int width, unsigned int height);

	void OnModelChanged();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

private:
	void Move();
	void Rotate();
	void Orbit();
	void Zoom();
	void Focus();
	void DetermineMouseInputState();
	void ExecuteUnfocus();
	void DetectFocus();
	void ExecuteFocus();
	void SetupFocus(float3 position, float3 size);
	void SetupFocus(float3 position, float bounding_sphere_radius);
};

#pragma once

#include "Module.h"
#include "Event.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/Quat.h"

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
	Entity*										camera_entity;
	ComponentCamera*							camera;
	ComponentTransform*							transform;
	camera_state								state;
	camera_mouse_input_state					mouse_input_state;
	bool										focus_on_model_changed;
	float										sensitivity = 10.f;
	float										orbit_speed;
	float										zoom_speed;
	float										zoom_velocity;
	float										zoom_drag;
	float										focus_duration;
	float										focus_lerp_position;
	float										focus_target_radius;
	math::float3								movement_speed;
	math::float3								fast_movement_speed;
	math::float3								rotation_euler;
	math::float3								focus_start_position;
	math::float3								focus_start_direction;
	math::float3								focus_target_position;
	math::float3								focus_destination_position;
	math::float3								focus_target_direction;
	math::Quat									focus_target_orientation;
	math::Quat									focus_start_orientation;
	EventListener<unsigned int, unsigned int>	window_resized_event_listener;

public:
	ModuleCamera();
	~ModuleCamera() override;
	
	bool Init() override;
	bool CleanUp() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	ComponentCamera* const GetCamera() const;
	ComponentTransform* const GetTransform() const;

	void HandleWindowResized(unsigned int width, unsigned int height);

private:
	void Move();
	void Rotate();
	void Orbit();
	void Zoom();
	void Focus();
	void DetermineMouseInputState();
	void DetectFocus();
	void ExecuteFocus();
	void SetupFocus(math::float3 position, math::float3 size);
	void SetupFocus(math::float3 position, float bounding_sphere_radius);
};
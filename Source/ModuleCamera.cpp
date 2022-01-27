#include "Globals.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleShaderProgram.h"

#include "Entity.h"
#include "ComponentBoundingBox.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "MATH_GEO_LIB/Geometry/Sphere.h"

ModuleCamera::ModuleCamera() : 
	state(camera_state::UNFOCUSED),
	mouse_input_state(camera_mouse_input_state::IDLE),
	sensitivity(0.0f),
	orbit_speed(0.0f),
	zoom_speed(0.0f),
	zoom_velocity(0.0f),
	zoom_drag(0.0f),
	movement_speed(0.0f),
	fast_movement_speed(0.0f),
	focus_on_model_changed(false),
	focus_start_position(math::float3::zero),
	focus_start_direction(math::float3::zero),
	focus_start_orientation(),
	focus_target_orientation(),
	focus_target_position(math::float3::zero),
	focus_destination_position(math::float3::zero),
	focus_target_direction(math::float3::zero),
	focus_duration(0.0f),
	focus_lerp_position(0.0f),
	focus_target_radius(0.0f),
	window_resized_event_listener(EventListener<unsigned int, unsigned int>()),
	camera_entity(nullptr), 
	transform(nullptr), 
	camera(nullptr)
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	// Initialize Camera:
	camera_entity = new Entity();
	camera_entity->Initialize("Editor Camera");
	camera_entity->AddComponent<ComponentCamera>();

	// Cache camera and transform components in variables
	// to have easy access:
	transform = camera_entity->Transform();
	camera = camera_entity->GetComponent<ComponentCamera>();

	// Set initial position:
	transform->SetPosition(float3(30.0f, 30.0f, 30.0f));

	// Set camera as the main camera:
	camera->SetIsMainCamera(true);

	// Set focus related variables:
	state = camera_state::UNFOCUSED;
	sensitivity = 10.0f;
	focus_on_model_changed = false;
	focus_target_position = -float3::unitX;
	focus_destination_position = float3::zero;
	focus_target_orientation = math::Quat::identity;
	focus_start_orientation = math::Quat::identity;
	focus_target_direction = float3::zero;
	focus_duration = 0.4f;
	focus_lerp_position = 0.0f;
	focus_target_radius = 0.0f;
	movement_speed = float3::one * 20.f;
	fast_movement_speed = float3::one * 40.f;
	orbit_speed = 1.0f;

	// Set zoom related variables:
	zoom_velocity = 0.0f;
	zoom_drag = 0.05;
	zoom_speed = 1.5f;

	// Look at focus_target_position from our position:
	camera->LookAt(focus_target_position - transform->GetPosition());

	// Initialize window resized event lister:
	window_resized_event_listener = EventListener<unsigned int, unsigned int>(std::bind(&ModuleCamera::HandleWindowResized, this, std::placeholders::_1, std::placeholders::_2));
	// Subscribe to window resized event of ModuleInput:
	App->input->GetWindowResizedEvent()->AddListener(&window_resized_event_listener);

	return true;
}

bool ModuleCamera::CleanUp()
{
	delete camera;

	// TODO(baran): Move this into a private method.
	// Unsubscribe from window resized if it's not null:
	Event<unsigned int, unsigned int>* window_resized_event = App->input->GetWindowResizedEvent();
	if (window_resized_event != nullptr)
	{
		window_resized_event->RemoveListener(&window_resized_event_listener);
	}

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	Move();

	Focus();

	Zoom();

	DetermineMouseInputState();

	Rotate();

	Orbit();

	camera_entity->PreUpdate();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
	camera_entity->Update();

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
	camera_entity->PostUpdate();

	return update_status::UPDATE_CONTINUE;
}

ComponentCamera* const ModuleCamera::GetCamera() const
{
	return camera;
}

ComponentTransform* const ModuleCamera::GetTransform() const
{
	return transform;
}

void ModuleCamera::HandleWindowResized(unsigned int width, unsigned int height)
{
	camera->SetAspectRatio((float)width / (float)height);
}

void ModuleCamera::OnModelChanged()
{
	// Initialize the size of the model to 10 by default:
	float size = 10.0f;

	// NOTE: This code assumes renderer will always return an Entity that is not 
	// a nullptr.
	ComponentBoundingBox* bounding_box = App->renderer->GetLoadedModel()->GetComponent<ComponentBoundingBox>();

	if (bounding_box != nullptr)
	{
		size = bounding_box->GetMinimalEnclosingSphereRadius();
	}

	float new_far_plane_distance = camera->GetFarPlaneDistance();

	new_far_plane_distance = math::Max(size * 10, new_far_plane_distance);

	camera->SetFarPlaneDistance(new_far_plane_distance);

	focus_on_model_changed = true;
}

void ModuleCamera::Move()
{
	// If the camera is focusing on a target right now, ignore movement through key inputs:
	if (state == camera_state::FOCUSING)
	{
		return;
	}

	// If user is not clicking on right mouse button, ignore movement through key inputs:
	if (!App->input->GetMouseKey(SDL_BUTTON_RIGHT, key_state::REPEAT))
	{
		return;
	}

	const float3 current_movement_speed =
		App->input->GetKey(SDL_SCANCODE_LSHIFT, key_state::REPEAT) ?
		fast_movement_speed :
		movement_speed;

	const float3 velocity = Time->DeltaTime() * current_movement_speed /** App->renderer->GetLoadedModel()->GetMinimalEnclosingSphereRadius()*0.1f*/;

	// Store and Cache Position:
	float3 new_position = transform->GetPosition();

	bool moved_this_frame = false;
	bool should_unfocus = false;

	if (App->input->GetKey(SDL_SCANCODE_W, key_state::REPEAT))
	{
		new_position += -transform->GetFront() * velocity.z;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S, key_state::REPEAT))
	{
		new_position -= -transform->GetFront() * velocity.z;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D, key_state::REPEAT))
	{
		new_position += transform->GetRight() * velocity.x;
		moved_this_frame = true;
		should_unfocus = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_A, key_state::REPEAT))
	{
		new_position -= transform->GetRight() * velocity.x;
		moved_this_frame = true;
		should_unfocus = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP, key_state::REPEAT))
	{
		new_position += transform->GetUp() * velocity.y;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN, key_state::REPEAT))
	{
		new_position -= transform->GetUp() * velocity.y;
		moved_this_frame = true;
	}

	if (moved_this_frame)
	{
		// Apply position changes:
		transform->SetPosition(new_position);
	}

	if (should_unfocus)
	{
		state = camera_state::UNFOCUSED;
	}
}

void ModuleCamera::Rotate()
{
	// If the camera is focusing on a target right now, ignore rotation through mouse movement:
	if (state == camera_state::FOCUSING)
	{
		return;
	}

	// If mouse_input_state is not ROTATE, return:
	if (mouse_input_state != camera_mouse_input_state::ROTATE)
	{
		return;
	}

	// Get mouse displacement from ModuleInput:
	float2 mouse_delta = App->input->GetMouseDisplacement();

	// Don't rotate if no mouse movement were made:
	if (mouse_delta.Equals(float2::zero))
	{
		return;
	}

	// Store smoothened and adjusted displacements of mouse as radians:
	float x_delta = math::DegToRad(mouse_delta.x * sensitivity * Time->DeltaTime());
	float y_delta = math::DegToRad(mouse_delta.y * sensitivity * Time->DeltaTime());

	math::float3 look_at_direction = -transform->GetFront();

	look_at_direction = Quat(transform->GetRight(), -y_delta) * look_at_direction;
	look_at_direction = Quat(transform->GetUp(), -x_delta) * look_at_direction;

	// Look At the look_at_direction:
	transform->LookAt(look_at_direction);

	// Unfocus the camera since a movement was made:
	state = camera_state::UNFOCUSED;
}

void ModuleCamera::Orbit()
{
	// If the camera is not focused on a target right now, ignore orbiting through mouse movement:
	if (state != camera_state::FOCUSED)
	{
		return;
	}

	// If mouse_input_state is not ORBIT, return:
	if (mouse_input_state != camera_mouse_input_state::ORBIT)
	{
		return;
	}

	// Get mouse displacement from ModuleInput:
	float2 mouse_delta = App->input->GetMouseDisplacement();

	// Store smoothened and adjusted displacements of mouse as radians:
	float x_delta = math::DegToRad(mouse_delta.x * sensitivity * Time->DeltaTime());
	float y_delta = math::DegToRad(mouse_delta.y * sensitivity * Time->DeltaTime());

	// Get orbit center:
	const float3 center_position = focus_target_position;

	// Calculate direction from position to orbit center:
	float3 direction = center_position - transform->GetPosition();
	// Rotate around camera's up by x_delta:
	Quat rotate_up = Quat::RotateAxisAngle(transform->GetUp(), -x_delta);
	// Rotate around camera's right by y_delta:
	Quat rotate_right = Quat::RotateAxisAngle(transform->GetRight(), -y_delta);

	// Apply rotations to direction:
	direction = (rotate_up * rotate_right).Transform(direction);
	// Update position:
	transform->SetPosition(center_position - direction);
	// Look at selected target:
	transform->LookAt(direction);

	// Update focus_target_direction to be the direction camera currently 
	// facing:
	focus_target_direction = transform->GetDirection();
}

void ModuleCamera::Zoom()
{
	// Calculate new speed by mouse wheel input:
	float input_velocity = App->input->GetMouseWheelDisplacement().y * zoom_speed * Time->DeltaTime();

	// If there is no mouse wheel input, dont touch the current zoom_velocity,
	// if there is a mouse input, override the zoom_velocity if the input_velocity
	// is bigger in size at the same direction, or input_velocity is to the opposite
	// direction of zoom_velocity:
	if (input_velocity != 0.0f)
	{
		if (math::Sign(input_velocity) == math::Sign(zoom_velocity))
		{
			input_velocity = math::Abs(zoom_velocity) > math::Abs(input_velocity) ? zoom_velocity : input_velocity;
		}

		zoom_velocity = input_velocity;
	}

	// If zoom velocity is 0, return:
	if (zoom_velocity == 0.0f)
	{
		return;
	}

	// Get current zoom drag:
	float current_zoom_drag = zoom_drag * Time->DeltaTime();

	// Set sign flipped to the sign of zoom_velocity:
	bool sign_flipped = (zoom_velocity > 0.0f);

	// Apply current_zoom_drag from the opposite direction of
	// zoom_velocity:
	if (zoom_velocity > 0.0f)
	{
		zoom_velocity -= current_zoom_drag;
	}
	else if (zoom_velocity < 0.0f)
	{
		zoom_velocity += current_zoom_drag;
	}

	// Set sign_flipped to true if the sign of the zoom_velocity
	// is different after the applied drag:
	sign_flipped = sign_flipped != (zoom_velocity > 0.0f);

	// If sign of the velocity is changed in the same function call,
	// halt the zooming, and set the zoom_velocity to zero:
	if (sign_flipped)
	{
		zoom_velocity = 0.0f;
		return;
	}

	// Set new_horizontal_fov to the current horizontal fov + zoom_velocity:
	float new_horizontal_fov = camera->GetHorizontalFOV() + zoom_velocity;

	static const float DEGREE_120_RADIANS = math::DegToRad(120.0f);
	static const float DEGREE_1_RADIANS = math::DegToRad(1.0f);

	// Clamp new_horizontal_fow between 1.0 degrees and 120.0 degrees:
	new_horizontal_fov = math::Clamp(new_horizontal_fov, DEGREE_1_RADIANS, DEGREE_120_RADIANS);

	// Set new horizontal fov value:
	camera->SetHorizontalFOV(new_horizontal_fov);
}

void ModuleCamera::Focus()
{
	DetectFocus();
	ExecuteFocus();
}

void ModuleCamera::DetermineMouseInputState()
{
	// Because of the ordering, ROTATE has superiority to
	// orbit, i.e if user clicks RMB, LMB and LALT at the same
	// time, camera won't orbit, it will rotate.

	// If user is clicking on right mouse button, set mouse_input_state to rotate mode:
	if (App->input->GetMouseKey(SDL_BUTTON_RIGHT, key_state::REPEAT))
	{
		mouse_input_state = camera_mouse_input_state::ROTATE;
		return;
	}

	// If user is clicking on left mouse button and left alt,
	// set mouse_input_state to rotate mode:
	if (App->input->GetMouseKey(SDL_BUTTON_LEFT, key_state::REPEAT) &&
		App->input->GetKey(SDL_SCANCODE_LALT, key_state::REPEAT))
	{
		mouse_input_state = camera_mouse_input_state::ORBIT;
		return;
	}

	// If none of the modes are detected, set mouse_input_state to idle:
	mouse_input_state = camera_mouse_input_state::IDLE;
}

void ModuleCamera::DetectFocus()
{
	if (App->input->GetKey(SDL_SCANCODE_F, key_state::DOWN) ||  focus_on_model_changed)
	{
		focus_on_model_changed = false;
		
		// Get Bounding Box component of the Entity:
		ComponentBoundingBox* bounding_box = App->renderer->GetLoadedModel()->GetComponent<ComponentBoundingBox>();
		
		// If Entity has no bounding box component attached to it, don't focus:
		if (bounding_box == nullptr)
		{
			return;
		}

		SetupFocus(bounding_box->GetCenterPosition(), bounding_box->GetMinimalEnclosingSphereRadius());
	}
}

void ModuleCamera::ExecuteFocus()
{
	if (state == camera_state::FOCUSED || state == camera_state::UNFOCUSED)
	{
		return;
	}

	focus_lerp_position += Time->DeltaTime() / focus_duration;

	// Stop Executing focus next frame if the lerp ended 
	// and clamp focus_lerp_position to be 1.0f at max:
	if (focus_lerp_position >= 1.0f)
	{
		focus_lerp_position = 1.0f;
		state = camera_state::FOCUSED;
	}

	// Look at the lerped direction:
	math::Quat orientation = math::Quat::Lerp(focus_start_orientation, focus_target_orientation, focus_lerp_position);
	// Set position to lerped position:
	transform->SetRotation(orientation);
	
	// Lerp to target position:
	float3 position = float3::Lerp(focus_start_position, focus_destination_position, focus_lerp_position);
	// Set position to lerped position:
	transform->SetPosition(position);
}

void ModuleCamera::SetupFocus(float3 position, float3 size)
{
	// Calculate a radius from max extent of given size:
	float bounding_sphere_radius = math::Max(size.x, math::Max(size.y, size.z)) * 0.5f;

	// Calculate camera positions from bounding_sphere_radius:
	SetupFocus(position, bounding_sphere_radius);
}

void ModuleCamera::SetupFocus(float3 position, float bounding_sphere_radius)
{
	if ((state == camera_state::FOCUSING || state == camera_state::FOCUSED) && position.Equals(focus_destination_position) && focus_target_radius == bounding_sphere_radius)
	{
		return;
	}

	// Get minimum of horizontal fov and vertical fov and divide it to half:
	float fov_min = math::Min(camera->GetHorizontalFOV(), camera->GetVerticalFOV());
	float fov_min_half = fov_min * 0.5f;

	// Calculate desired distance from center of the sphere to the camera:
	// This can be visualized as drawing a straight line from camera to the center of the sphere and a
	// tangent from the camera to the sphere. Resulting tangent will have angle of fov_min_half between 
	// that straigth line. Then drawing a line from center of the sphere to that tangent's intersection 
	// point, that will form a right angle to the tangent. The resulting right-angled triangle will have
	// hypotenuse as desired_distance, and bounding_sphere_radius as the side of the triangle
	// that is in front of the fov_min_half angle.
	// So we have sin(fov_min_half) = bounding_sphere_radius / desired_distance, which by simple
	// algebra, will be desired_distance = bounding_sphere_radius / sin(fov_min_half).
	float desired_distance = math::Abs(bounding_sphere_radius / math::Sin(fov_min_half));

	// Set state of camera to FOCUSING:
	state = camera_state::FOCUSING;
	// Set starting position:
	focus_start_position = transform->GetPosition();
	// Set starting direction:
	focus_start_direction = transform->GetDirection();
	// Set starting orientation:
	focus_start_orientation = transform->GetRotation();
	// Set target position:
	focus_target_position = position;
	// Set focus_target_radius to given bounding_sphere_radius:
	focus_target_radius = bounding_sphere_radius;
	// Get target direction (Actually it's direction from target to camera):
	focus_target_direction = (focus_target_position - focus_start_position).Normalized();
	// Get target orientation:
	focus_target_orientation = transform->SimulateLookAt(focus_target_direction);
	// Set Focus destination position to the position away from target's position by desired_distance:
	focus_destination_position = focus_target_position - desired_distance * focus_target_direction;
	// Set focus_lerp_position to 0:
	focus_lerp_position = 0.0f;
}
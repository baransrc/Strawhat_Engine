#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleShaderProgram.h"
#include "Model.h"
#include "Globals.h"
#include "MathGeoLib.h"
#include "MATH_GEO_LIB/Geometry/Sphere.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	// Initialize the frustum:
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);

	// TODO: Make this switchable inside the editor:
	if (is_perspective)
	{
		SetAsPerspective
		(
			math::DegToRad(45.0f), // Horizontal field of view
			(float)App->window->window_width / (float)App->window->window_height // Aspect ratio
		);
	}
	else
	{
		SetAsOrthographic(5.f, 5.f);
	}

	SetPlaneDistances(0.1f, 200.0f);

	// Set model matrix to identity as we are staying still:
	model_matrix = float4x4::identity;

	translation_matrix = float4x4::identity;
	rotation_matrix = float4x4::identity;

	SetPosition(float3(10.0f, 10.0f, 10.0f));

	state = camera_state::UNFOCUSED;
	
	focus_on_model_changed = true;
	focus_target_position = -float3::unitX;
	focus_destination_position = float3::zero;
	focus_duration = 0.65f;
	focus_lerp_position = 1.0f;
	focus_target_direction = float3::zero;
	focus_target_radius = 0.0f;

	movement_speed = float3::one * 20.f;
	fast_movement_speed = float3::one * 40.f;
	orbit_speed = 1.0f;
	
	// Set zoom related variables:
	zoom_velocity = 0.0f;
	zoom_drag = 0.05;
	zoom_speed = 1.5f;

	// Look at focus_target_position from our position:
	LookAt(focus_target_position, vector_mode::POSITION, true);

	// Get view matrix after new calculations:
	ComputeViewMatrix();

	// No need to calculate projection matrix as it will be calculated in the PreUpdate method.
	
	return true;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

void ModuleCamera::SetHorizontalFOV(float new_horizontal_fov)
{
	frustum.SetHorizontalFovAndAspectRatio(new_horizontal_fov, GetAspectRatio());
	should_recalculate_projection_matrix = true;
}

void ModuleCamera::SetAspectRatio(float new_aspect_ratio)
{
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), new_aspect_ratio);
	should_recalculate_projection_matrix = true;
}

void ModuleCamera::SetFarPlaneDistance(float new_far_plane_distance)
{
	frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), new_far_plane_distance);
	should_recalculate_projection_matrix = true; // NOTE: Should far plane effect projection matrix?
}

void ModuleCamera::SetNearPlaneDistance(float new_near_plane_distance)
{
	frustum.SetViewPlaneDistances(new_near_plane_distance, frustum.FarPlaneDistance());
	should_recalculate_projection_matrix = true; // NOTE: Should near plane effect projection matrix?
}

void ModuleCamera::SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance)
{
	frustum.SetViewPlaneDistances(new_near_plane_distance, new_far_plane_distance); 
	should_recalculate_projection_matrix = true; // NOTE: Should near and far plane effect projection matrix?
}

void ModuleCamera::SetPosition(float3 new_position)
{
	frustum.SetPos(new_position);

	translation_matrix[0][3] = -new_position.x;
	translation_matrix[1][3] = -new_position.y;
	translation_matrix[2][3] = -new_position.z;
}

void ModuleCamera::SetRotation(float3 new_orientation)
{
	Quat rotation = Quat::FromEulerXYZ(new_orientation.x, new_orientation.y, new_orientation.z);

	rotation_matrix = rotation * float4x4::identity;
}

void ModuleCamera::SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio)
{
	frustum.SetPerspective(new_horizontal_fov, 2 * math::Atan(math::Tan(new_horizontal_fov * 0.5f) * new_aspect_ratio));
	SetAspectRatio(new_aspect_ratio);
	should_recalculate_projection_matrix = true;
}

float3 ModuleCamera::GetUp() const
{
	return float3(rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2]);
}

float3 ModuleCamera::GetFront() const
{
	return -GetDirection();
}

float3 ModuleCamera::GetDirection() const
{
	return float3(rotation_matrix[2][0], rotation_matrix[2][1], rotation_matrix[2][2]);
}

float3 ModuleCamera::GetRight() const
{
	return float3(rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2]);
}

float3 ModuleCamera::GetPosition() const
{
	return float3(-translation_matrix[0][3], -translation_matrix[1][3], -translation_matrix[2][3]);
}

float3 ModuleCamera::GetRotation() const
{
	return rotation_euler;
}

void ModuleCamera::SetUp(float3 new_up)
{
	rotation_matrix[1][0] = new_up.x;
	rotation_matrix[1][1] = new_up.y; 
	rotation_matrix[1][2] = new_up.z;
}

void ModuleCamera::SetFront(float3 new_front)
{
	SetDirection(-new_front);
}

void ModuleCamera::SetDirection(float3 new_direction)
{
	rotation_matrix[2][0] = new_direction.x;
	rotation_matrix[2][1] = new_direction.y;
	rotation_matrix[2][2] = new_direction.z;
}

void ModuleCamera::SetRight(float3 new_right)
{
	rotation_matrix[0][0] = new_right.x;
	rotation_matrix[0][1] = new_right.y;
	rotation_matrix[0][2] = new_right.z;
}

void ModuleCamera::SetAsOrthographic(float new_orthographic_width, float new_orthographic_height)
{
	frustum.SetOrthographic(new_orthographic_width, new_orthographic_height);
	should_recalculate_projection_matrix = true;
}

/// <summary>
/// This method only computes direction, front, right and up according to specified position/direction.
/// Actual Computation of view matrix according to those properties are done in ComputeViewMatrix method.
/// </summary>
/// <param name="look_at">: Position/Direction to look at.</param>
/// <param name="interpret_as">: Interpret look_at as either DIRECTION or POSITION</param>
/// <param name="calculate_rotation">: Should camera recalculate it's stored rotation angles according to the calculated direction vector?</param>
void ModuleCamera::LookAt(float3 look_at, vector_mode interpret_as, bool calculate_rotation)
{
	float3 new_direction = float3::zero;

	if (interpret_as == vector_mode::POSITION)
	{
		new_direction = (GetPosition() - look_at).Normalized();
	}
	else if (interpret_as == vector_mode::DIRECTION)
	{
		new_direction = look_at.Normalized();
	}

	SetDirection(new_direction);

	SetRight((float3::unitY.Cross(GetDirection())).Normalized());
	
	SetUp(((GetDirection()).Cross(GetRight())).Normalized());

	if (calculate_rotation)
	{
		CalculateRotationFromDirection();
	}
}

/// <summary>
/// Computes view matrix using current position, right, up and direction.
/// </summary>
void ModuleCamera::ComputeViewMatrix()
{
	view_matrix = Quat(rotation_matrix).Normalized() * (translation_matrix);
}

void ModuleCamera::AutoRotateAround(float3 position)
{
	const float radius = 10.0f;
	static float angle = 0.0f;

	angle += Time->DeltaTime() * orbit_speed;

	float cam_x = sin(angle) * radius;
	float cam_z = cos(angle) * radius;
	float cam_y = GetPosition().y;

	SetPosition(float3(cam_x, cam_y, cam_z));

	LookAt(position);
}

void ModuleCamera::WindowResized(unsigned int width, unsigned int height)
{
	SetAspectRatio((float)width / (float)height);
}

void ModuleCamera::OnModelChanged()
{
	float size =  App->renderer->GetLoadedModel()->GetMinimalEnclosingSphereRadius();

	float new_far_plane_distance = frustum.FarPlaneDistance();

	new_far_plane_distance = math::Max(size * 10, new_far_plane_distance);

	SetFarPlaneDistance(new_far_plane_distance);

	focus_on_model_changed = true;
}

update_status ModuleCamera::PreUpdate()
{
	if (should_auto_rotate_around_target)
	{
		AutoRotateAround(focus_destination_position);
	}

	Move();
	Focus();
	Zoom();
	
	DetermineMouseInputState();
	Rotate();
	Orbit();
	
	// Recalculate projection matrix if necessary:
	if (should_recalculate_projection_matrix)
	{
		should_recalculate_projection_matrix = false;
		CalculateProjectionMatrix();
	}
	
	ComputeViewMatrix();

	// Make sure we are using the true shader before passing the arguments:
	App->shader_program->Use();

	// Pass transposed model view projection matrices to the shader, as MathGeoLib is row major
	// and OpenGL is column major:
	App->shader_program->SetUniformVariable("model_matrix", model_matrix, true);
	App->shader_program->SetUniformVariable("view_matrix", view_matrix, true);
	App->shader_program->SetUniformVariable("projection_matrix", projection_matrix, true);

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCamera::Update()
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleCamera::PostUpdate()
{
	return update_status::UPDATE_CONTINUE;
}

void ModuleCamera::CalculateRotationFromDirection()
{
	float3 direction = GetDirection().Normalized();

	rotation_euler.x = acos(direction.x);
	rotation_euler.y = acos(direction.y);
	rotation_euler.z = acos(direction.z);
}

void ModuleCamera::CalculateProjectionMatrix()
{
	frustum.ComputeProjectionMatrix();
	projection_matrix = frustum.ProjectionMatrix();
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

	const float3 velocity = Time->DeltaTime() * current_movement_speed * App->renderer->GetLoadedModel()->GetMinimalEnclosingSphereRadius()*0.1f;

	// Store and Cache Position:
	float3 new_position = GetPosition();

	bool moved_this_frame = false;
	bool should_unfocus = false;

	if (App->input->GetKey(SDL_SCANCODE_W, key_state::REPEAT))
	{
		new_position += GetFront() * velocity.z;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S, key_state::REPEAT))
	{
		new_position -= GetFront() * velocity.z;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D, key_state::REPEAT))
	{
		new_position += GetRight() * velocity.x;
		moved_this_frame = true;
		should_unfocus = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_A, key_state::REPEAT))
	{
		new_position -= GetRight() * velocity.x;
		moved_this_frame = true;
		should_unfocus = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP, key_state::REPEAT))
	{
		new_position += GetUp() * velocity.y;
		moved_this_frame = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN, key_state::REPEAT))
	{
		new_position -= GetUp() * velocity.y;
		moved_this_frame = true;
	}
	
	if (moved_this_frame)
	{
		// Apply position changes:
		SetPosition(new_position);
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

	// Store rotations in rotation_euler:
	rotation_euler.x += x_delta;
	rotation_euler.y += y_delta;

	// NOTE: The following calculations may be done by rotating an identity matrix
	// with current rotations of camera, and then assigning the formed matrix to 
	// rotation_matrix. Current solution seems a lot more smoother, and it basically
	// ignores roll a.k.a rotation around z-axis.

	// Rotate around x axis:
	Quat rotation_quaternion = Quat(float3::unitX, y_delta);
	// Rotate around y axis and combine with the previous rotation around x axis:
	rotation_quaternion = rotation_quaternion * Quat(float3::unitY, x_delta);

	// Apply rotation to rotation_matrix:
	rotation_matrix = rotation_quaternion * rotation_matrix;

	// Recalculate up and right of camera according to new direction, but no need 
	// to recalculate stored rotation angles:
	LookAt(GetDirection(), vector_mode::DIRECTION, false);

	// Unfocus the camera since a movement was made:
	state = camera_state::UNFOCUSED;
}

void ModuleCamera::Orbit()
{
	// If the camera is focusing on a target right now, ignore orbiting through mouse movement:
	if (state == camera_state::FOCUSING)
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
	float3 direction = GetPosition() - center_position;

	// Rotate around camera's up by x_delta:
	Quat rotate_up = Quat::RotateAxisAngle(GetUp(), x_delta);
	// Rotate around camera's right by y_delta:
	Quat rotate_right = Quat::RotateAxisAngle(GetRight(), y_delta);
	
	// Apply rotations to direction:
	direction = (rotate_up * rotate_right).Transform(direction);
	
	// Update position:
	SetPosition(center_position + direction);
	
	// Look at selected target:
	LookAt(center_position);

	// Update focus_target_direction to be the direction camera currently 
	// facing:
	focus_target_direction = GetDirection();
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
	float new_horizontal_fov = frustum.HorizontalFov() + zoom_velocity;

	static const float DEGREE_120_RADIANS = math::DegToRad(120.0f);
	static const float DEGREE_1_RADIANS = math::DegToRad(1.0f);

	// Clamp new_horizontal_fow between 1.0 degrees and 120.0 degrees:
	new_horizontal_fov = math::Clamp(new_horizontal_fov, DEGREE_1_RADIANS, DEGREE_120_RADIANS);

	// Set new horizontal fov value:
	SetHorizontalFOV(new_horizontal_fov);
}

void ModuleCamera::Focus()
{
	DetectFocus();
	ExecuteFocus();
	ExecuteUnfocus();
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

void ModuleCamera::ExecuteUnfocus()
{
	if (mouse_input_state == camera_mouse_input_state::ORBIT)
	{
		return;
	}

	if (state != camera_state::UNFOCUSED)
	{
		return;
	}

	focus_lerp_position = 1.0f;
	focus_target_position = GetPosition() + 10.0f * GetFront();
	focus_destination_position = GetPosition();
	focus_target_direction = GetFront();
	focus_target_radius = 0.0f;
}

void ModuleCamera::DetectFocus()
{
	if (App->input->GetKey(SDL_SCANCODE_F, key_state::DOWN) ||  focus_on_model_changed)
	{
		focus_on_model_changed = false;
		SetupFocus(App->renderer->GetLoadedModel()->GetCenterPosition(), App->renderer->GetLoadedModel()->GetMinimalEnclosingSphereRadius());
	}
}

void ModuleCamera::ExecuteFocus()
{
	if (state == camera_state::FOCUSED || state == camera_state::UNFOCUSED)
	{
		return;
	}

	focus_lerp_position += Time->DeltaTime() / focus_duration; // TODO: Create setter for focus_duration and cache 1/focus_duration.

	// Stop Executing focus next frame if the lerp ended 
	// and clamp focus_lerp_position to be 1.0f at max:
	if (focus_lerp_position >= 1.0f)
	{
		focus_lerp_position = 1.0f;
		state = camera_state::FOCUSED;
	}

	LOG("Executing Focus: %f%%", 100.f * focus_lerp_position);

	// Lerp to target direction:
	float3 direction = float3::Lerp(GetDirection(), focus_target_direction, focus_lerp_position).Normalized();
	// Look at the lerped direction:
	LookAt(direction, vector_mode::DIRECTION);

	// Lerp to target position:
	float3 position = float3::Lerp(GetPosition(), focus_destination_position, focus_lerp_position);
	// Set position to lerped position:
	SetPosition(position);
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
	float fov_min = math::Min(frustum.HorizontalFov(), frustum.VerticalFov());
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

	// Set target position:
	focus_target_position = position;

	// Set focus_target_radius to given bounding_sphere_radius:
	focus_target_radius = bounding_sphere_radius;

	// Get target direction (Actually it's direction from target to camera):
	focus_target_direction = (GetPosition() - focus_target_position).Normalized();

	// Set Focus destination position to the position away from target's position by desired_distance:
	focus_destination_position = focus_target_position + desired_distance * focus_target_direction;

	// Set focus_lerp_position to 0:
	focus_lerp_position = 0.0f;
}
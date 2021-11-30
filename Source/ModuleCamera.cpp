#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleShaderProgram.h"
#include "MathGeoLib.h"
#include "Globals.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	// TODO: Inside the editor window make this changeable too:
	SetPosition(float3(0.0f, 1.0f, 10.0f));

	target_position = float3::zero;
	movement_speed = float3::one * 0.5f;
	orbit_speed = 1.0f;

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

	// Look at position 0,0,0 from our position:
	LookAt(float3(0.f, 0.f, 0.f));

	float3 angles = view_matrix.ToEulerXYZ();

	pitch = angles.x;
	yaw = angles.y;

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
}

void ModuleCamera::SetOrientation(float3 new_orientation)
{
	// TODO: Implement this.
}

void ModuleCamera::SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio)
{
	frustum.SetPerspective(new_horizontal_fov, 2 * math::Atan(math::Tan(new_horizontal_fov * 0.5f) * new_aspect_ratio));
	SetAspectRatio(new_aspect_ratio);
	should_recalculate_projection_matrix = true;
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
void ModuleCamera::LookAt(float3 look_at, vector_mode interpret_as)
{
	if (interpret_as == vector_mode::POSITION)
	{
		direction = (GetPosition() - look_at).Normalized();
	}
	else if (interpret_as == vector_mode::DIRECTION)
	{
		direction = look_at.Normalized();
	}

	front = -1.0f * direction;

	right = (float3::unitY.Cross(direction)).Normalized();
	
	up = (direction.Cross(right)).Normalized();
}

/// <summary>
/// Computes view matrix using current position, right, up and direction.
/// </summary>
void ModuleCamera::ComputeViewMatrix()
{
	float3 current_position = GetPosition();

	translate_matrix = float4x4
	(
		1.0f, 0.0f, 0.0f, -1.0f * current_position.x,
		0.0f, 1.0f, 0.0f, -1.0f * current_position.y,
		0.0f, 0.0f, 1.0f, -1.0f * current_position.z,
		0.0f, 0.0f, 0.0f,  1.0f
	);

	rotation_matrix = float4x4
	(
		right.x,		right.y,		right.z,		0.0f,
		up.x,			up.y,			up.z,			0.0f,
		direction.x,	direction.y,	direction.z,	0.0f,
		0.0f,			0.0f,			0.0f,			1.0f
	);

	// TODO: Maybe combine these two matrices offline, to optimize this multiplication:
	view_matrix = translate_matrix * rotation_matrix;
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

update_status ModuleCamera::PreUpdate()
{
	// Recalculate projection matrix if necessary:
	if (should_recalculate_projection_matrix)
	{
		should_recalculate_projection_matrix = false;
		CalculateProjectionMatrix();
	}

	if (should_auto_rotate_around_target)
	{
		AutoRotateAround(target_position);
	}

	ToggleLock();
	if (!locked)
	{
		Move();
		Rotate();
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

void ModuleCamera::CalculateProjectionMatrix()
{
	frustum.ComputeProjectionMatrix();
	projection_matrix = frustum.ProjectionMatrix();
}

void ModuleCamera::Move()
{
	const float3 velocity = Time->DeltaTime() * movement_speed;

	// Store and Cache Position:
	float3 new_position = GetPosition();

	if (App->input->GetKey(SDL_SCANCODE_W, key_state::REPEAT))
	{
		new_position += front * movement_speed.z;
	}
	if (App->input->GetKey(SDL_SCANCODE_S, key_state::REPEAT))
	{
		new_position -= front * movement_speed.z;
	}
	if (App->input->GetKey(SDL_SCANCODE_D, key_state::REPEAT))
	{
		// Camera's right * movement_speed.x
		new_position += (front.Cross(up)).Normalized() * movement_speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_A, key_state::REPEAT))
	{
		// -Camera's right * movement_speed.x
		new_position -= (front.Cross(up)).Normalized() * movement_speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP, key_state::REPEAT))
	{
		new_position += up * movement_speed.y;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN, key_state::REPEAT))
	{
		new_position -= up * movement_speed.y;
	}

	// Apply position changes:
	SetPosition(new_position);

	LookAt(new_position + direction);
}

void ModuleCamera::Rotate()
{
	float2 mouse_delta = App->input->GetMouseDisplacement();
	float x_offset = mouse_delta.x * sensitivity * Time->DeltaTime();
	float y_offset = mouse_delta.y * sensitivity * Time->DeltaTime();

	yaw += x_offset;
	pitch += y_offset;

	//// Clamp pitch between -89.0 and 89.0 to avoid gimbal lock.
	//pitch = pitch > 89.0f ? 89.0f : (pitch < -89.0f ? -89.0f : pitch);

	float x_delta = math::DegToRad(x_offset);
	float y_delta = math::DegToRad(y_offset);

	float yaw_radians = math::DegToRad(yaw);
	float pitch_radians = math::DegToRad(pitch);

	float3 new_direction = float3::zero;

	Quat quat = Quat(float3::unitX, pitch_radians);
	quat = quat * Quat(float3::unitY, yaw_radians);

	//new_direction.x = cos(yaw_radians) * cos(pitch_radians);
	//new_direction.y = sin(pitch_radians);
	//new_direction.z = sin(yaw_radians) * cos(pitch_radians);

	new_direction = quat * float3(0.f, -1.f, 0.f);
	
	direction = new_direction.Normalized();

	LookAt(direction, vector_mode::DIRECTION);
}

void ModuleCamera::ToggleLock()
{
	if (App->input->GetKey(SDL_SCANCODE_F10, key_state::DOWN))
	{
		locked = !locked;
		LookAt(target_position);
	}
}
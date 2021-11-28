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
	SetPosition(float3(2.0f, 3.0f, 10.0f));

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

	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);
	
	SetPlaneDistances(0.1f, 200.0f);

	// Set model matrix to identity as we are staying still:
	model_matrix = float4x4::identity;

	// Look at position 0,0,0 from our position:
	LookAt(float3(0.f, 0.f, 0.f));

	// Get view matrix after new calculations:
	view_matrix = frustum.ViewMatrix();

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

void ModuleCamera::LookAt(float3 look_at_position)
{
	// Calculate direction to the origin:
	float3 direction = (look_at_position - GetPosition()).Normalized();

	// Calculate the delta rotation when looking at origin position:
	float4x4 rotation_matrix = float4x4::LookAt(frustum.Front(), direction, frustum.Up(), float3::unitY);

	// Set front and up accordingly:
	frustum.SetFront(rotation_matrix.MulDir(frustum.Front().Normalized()));
	frustum.SetUp(rotation_matrix.MulDir(frustum.Up().Normalized()));

	view_matrix = frustum.ViewMatrix();
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

	view_matrix = frustum.ViewMatrix();
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

	Move();

	LookAt(frustum.Front() + GetPosition());

	if (should_auto_rotate_around_target)
	{
		AutoRotateAround(target_position);
	}

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
		new_position += frustum.Front() * movement_speed.z;
	}
	if (App->input->GetKey(SDL_SCANCODE_S, key_state::REPEAT))
	{
		new_position -= frustum.Front() * movement_speed.z;
	}
	if (App->input->GetKey(SDL_SCANCODE_D, key_state::REPEAT))
	{
		// Camera's right * movement_speed.x
		new_position += (frustum.Front().Cross(frustum.Up())).Normalized() * movement_speed.x;
	}
	if (App->input->GetKey(SDL_SCANCODE_A, key_state::REPEAT))
	{
		// -Camera's right * movement_speed.x
		new_position -= (frustum.Front().Cross(frustum.Up())).Normalized() * movement_speed.x;
	}

	// Apply position changes:
	SetPosition(new_position);

	LOG("Position: %f,%f,%f", GetPosition().x, GetPosition().y, GetPosition().z);
}

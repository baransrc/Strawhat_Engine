#include "Application.h"
#include "Globals.h"
#include "ModuleCamera.h"
#include "MathGeoLib.h"
#include "ModuleShaderProgram.h"

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

	// TODO: Calculate this dynamically when the screen is resized:
	SetAspectRatio((float)SCREEN_HEIGHT / (float)SCREEN_WIDTH);

	// Initialize the frustum:
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);

	// TODO: Make this switchable inside the editor:
	if (is_perspective)
	{
		SetAsPerspective(math::DegToRad(90.0f));
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
	float vertical_fov = 2 * math::Atan(math::Tan(frustum.HorizontalFov() * 0.5f) * new_aspect_ratio);
	frustum.SetVerticalFovAndAspectRatio(vertical_fov, new_aspect_ratio);
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

void ModuleCamera::SetAsPerspective(float new_horizontal_fov)
{
	frustum.SetPerspective(new_horizontal_fov, 2 * math::Atan(math::Tan(new_horizontal_fov * 0.5f) * GetAspectRatio()));
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
	float3 direction = (look_at_position - frustum.Pos()).Normalized();

	// Calculate the delta rotation when looking at origin position:
	float4x4 rotation_matrix = float4x4::LookAt(frustum.Front(), direction, frustum.Up(), float3::unitY);

	// Set front and up accordingly:
	frustum.SetFront(rotation_matrix.MulDir(frustum.Front().Normalized()));
	frustum.SetUp(rotation_matrix.MulDir(frustum.Up().Normalized()));
}

update_status ModuleCamera::PreUpdate()
{
	// Recalculate projection matrix if necessary:
	if (should_recalculate_projection_matrix)
	{
		should_recalculate_projection_matrix = false;
		CalculateProjectionMatrix();
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

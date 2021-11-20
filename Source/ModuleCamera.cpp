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
	// TODO: Make an editor window to change these:
	fov_vertical = math::pi * 0.25f;
	fov_horizontal = math::DegToRad(45.0f);
	orthographic_height = 5.f;
	orthographic_width = 5.f;
	near_plane = 0.1f;
	far_plane = 100.0f;

	// TODO: Inside the editor window make this changeable too:
	position = float3(1.0f, 1.0f, 3.0f);

	// TODO: Calculate this dynamically when the screen is resized:
	aspect_ratio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

	// Initialize the frustum:
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);

	// TODO: Make this switchable inside the editor:
	if (is_perspective)
	{
		frustum.SetPerspective(fov_horizontal, fov_vertical);
		frustum.SetVerticalFovAndAspectRatio(fov_vertical, aspect_ratio);
		frustum.SetHorizontalFovAndAspectRatio(fov_horizontal, aspect_ratio);
	}
	else
	{
		frustum.SetOrthographic(orthographic_width, orthographic_height);
	}

	frustum.SetPos(position);
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);
	frustum.SetViewPlaneDistances(near_plane, far_plane);
	
	// Set model matrix to identity as we are staying still:
	model_matrix = float4x4::identity;

	// Set projection matrix with the values:
	// If it's perspective, using fov_horizontal, fov_vertical, aspect_ratio, near_plane and far_plane.
	// If it's orthographic, using orthographic_width, orthographic_height, aspect_ratio, near_plane and far_plane.
	projection_matrix = frustum.ProjectionMatrix(); // TODO: Calculate this only when there is a change in related settings or window size.

	// Calculate direction to the origin:
	float3 direction = (float3(0.f, 0.f, 0.f) - position).Normalized();
	
	// Calculate the delta rotation when looking at origin position:
	float4x4 rotation_matrix = float4x4::LookAt(frustum.Front(), direction, frustum.Up(), float3::unitY);
	
	// Set front and up accordingly:
	frustum.SetFront(rotation_matrix.MulDir(frustum.Front()));
	frustum.SetUp(rotation_matrix.MulDir(frustum.Up()));
	
	// Get view matrix after new calculations:
	view_matrix = frustum.ViewMatrix();
	
	return true;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

update_status ModuleCamera::PreUpdate()
{
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

#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Entity.h"

#include "ModuleShaderProgram.h"
#include "ModuleWindow.h"
#include "Application.h"

ComponentCamera::ComponentCamera() : 
	Component(),
	frustum(math::Frustum()),
	projection_matrix(math::float4x4::identity),
	view_matrix(math::float4x4::identity),
	projection_mode(camera_projection_mode::ORTHOGRAPHIC),
	should_calculate_projection_matrix(false),
	is_main_camera(false),
	component_changed_event_listener(EventListener<component_type>())
{
}

ComponentCamera::~ComponentCamera()
{
	if (owner != nullptr)
	{
		owner->GetComponentsChangedEvent()->RemoveListener(&component_changed_event_listener);
		owner->GetComponentsChangedInDescendantsEvent()->RemoveListener(&component_changed_event_listener);
	}
}

void ComponentCamera::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);

	// Initialize the frustum:
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	
	SetAsPerspective(
		math::DegToRad(45.0f), // Horizontal field of view
		(float)App->window->window_width / (float)App->window->window_height // Aspect ratio
	);

	SetPlaneDistances(0.1f, 200.0f);

	UpdateTransformVariables();

	component_changed_event_listener = EventListener<component_type>(std::bind(&ComponentCamera::HandleComponentChanged, this, std::placeholders::_1));

	owner->GetComponentsChangedEvent()->AddListener(&component_changed_event_listener);
	owner->GetComponentsChangedInDescendantsEvent()->AddListener(&component_changed_event_listener);
}

void ComponentCamera::PreUpdate()
{
	// Calculate the projection matrix if it's necessary:
	CalculateProjectionMatrix();

	// If it's the main camera, use the matrices of this:
	// NOTE: When play/stop is implemented, we will start rendering from 
	// the camera that is flagged as main camera found in the current scene.
	if (is_main_camera)
	{
		// Make sure we are using the true shader before passing the arguments:
		App->shader_program->Use();

		// Pass transposed model view projection matrices to the shader, as MathGeoLib is row major
		// and OpenGL is column major:
		App->shader_program->SetUniformVariable("model_matrix", owner->Transform()->GetMatrix(), true);
		App->shader_program->SetUniformVariable("view_matrix", view_matrix, true);
		App->shader_program->SetUniformVariable("projection_matrix", projection_matrix, true);

		App->shader_program->SetUniformVariable("camera_position", owner->Transform()->GetPosition());
	}
}

void ComponentCamera::Update()
{

}

void ComponentCamera::DrawGizmo()
{
}

component_type ComponentCamera::Type() const
{
	return component_type::CAMERA;
}

const math::float4x4& ComponentCamera::GetViewMatrix() const
{
	return view_matrix;
}

const math::float4x4& ComponentCamera::GetProjectionMatrix() const
{
	return projection_matrix;
}

float ComponentCamera::GetVerticalFOV() const
{
	return frustum.VerticalFov();
}

float ComponentCamera::GetHorizontalFOV() const
{
	return frustum.HorizontalFov();
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

float ComponentCamera::GetOrthographicWidth() const
{
	return frustum.OrthographicWidth();
}

float ComponentCamera::GetOrthographicHeight() const
{
	return frustum.OrthographicHeight();
}

float ComponentCamera::GetFarPlaneDistance() const
{
	return frustum.FarPlaneDistance();
}

float ComponentCamera::GetNearPlaneDistance() const
{
	return frustum.NearPlaneDistance();
}

camera_projection_mode ComponentCamera::GetProjectionMode()
{
	return projection_mode;
}

void ComponentCamera::SetHorizontalFOV(float new_horizontal_fov)
{
	frustum.SetHorizontalFovAndAspectRatio(new_horizontal_fov, GetAspectRatio());
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetAspectRatio(float new_aspect_ratio)
{
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), new_aspect_ratio);
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetFarPlaneDistance(float new_far_plane_distance)
{
	frustum.SetViewPlaneDistances(frustum.NearPlaneDistance(), new_far_plane_distance);
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetNearPlaneDistance(float new_near_plane_distance)
{
	frustum.SetViewPlaneDistances(new_near_plane_distance, frustum.FarPlaneDistance());
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance)
{
	frustum.SetViewPlaneDistances(new_near_plane_distance, new_far_plane_distance);
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetIsMainCamera(bool new_is_main_camera)
{
	is_main_camera = new_is_main_camera;
}

void ComponentCamera::SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio)
{
	frustum.SetPerspective(new_horizontal_fov, 2 * math::Atan(math::Tan(new_horizontal_fov * 0.5f) * new_aspect_ratio));
	SetAspectRatio(new_aspect_ratio);
	projection_mode = camera_projection_mode::PERSPECTIVE;
	should_calculate_projection_matrix = true;
}

void ComponentCamera::SetAsOrthographic(float new_orthographic_width, float new_orthographic_height)
{
	frustum.SetOrthographic(new_orthographic_width, new_orthographic_height);
	projection_mode = camera_projection_mode::ORTHOGRAPHIC;
	should_calculate_projection_matrix = true;
}

void ComponentCamera::LookAt(const math::float3& direction)
{
	owner->Transform()->LookAt(direction.Normalized());
}

void ComponentCamera::DrawInspectorContent()
{

}

void ComponentCamera::CalculateProjectionMatrix()
{
	if (!should_calculate_projection_matrix)
	{
		return;
	}

	should_calculate_projection_matrix = false;

	frustum.ComputeProjectionMatrix();
	projection_matrix = frustum.ProjectionMatrix();
}

void ComponentCamera::UpdateTransformVariables()
{
	// Set frustum position & rotation to transform's values:
	// TODO: If there are issues with the frustum gizmo rendering, 
	// check this out, as this may need the values of view_matrix.
	frustum.SetPos(owner->Transform()->GetPosition());
	frustum.SetUp(owner->Transform()->GetUp());
	frustum.SetFront(owner->Transform()->GetFront());

	// Recalculate the view_matrix according to the changed transform:
	view_matrix = owner->Transform()->GetMatrix().Inverted();
}

void ComponentCamera::HandleComponentChanged(component_type type)
{
	// If the changed component is not transform component of owner,
	// don't care about it:
	if (type != component_type::TRANSFORM)
	{
		return;
	}

	// Update transform related variables:
	UpdateTransformVariables();
}
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Entity.h"
#include "Scene.h"

#include "ModuleDebugDraw.h"
#include "ModuleInput.h"
#include "ModuleShaderProgram.h"
#include "ModuleSceneManager.h"
#include "ModuleWindow.h"
#include "Application.h"

#include "MATH_GEO_LIB/Geometry/Plane.h"
#include "MATH_GEO_LIB/Geometry/LineSegment.h"

ComponentCamera::ComponentCamera() : 
	Component(),
	frustum(math::Frustum()),
	projection_matrix(math::float4x4::identity),
	view_matrix(math::float4x4::identity),
	projection_mode(camera_projection_mode::ORTHOGRAPHIC),
	should_calculate_projection_matrix(false),
	is_main_camera(false),
	should_render(false),
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

	// Set Should Render to true:
	should_render = true;

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
	if (!Enabled())
	{
		return;
	}

	// Calculate the projection matrix if it's necessary:
	CalculateProjectionMatrix();

	// If it's the main camera, use the matrices of this:
	// NOTE: When play/stop is implemented, we will start rendering from 
	// the camera that is flagged as main camera found in the current scene.
	if (is_main_camera && should_render)
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
	if (!Enabled())
	{
		return;
	}
}

void ComponentCamera::DrawGizmo()
{
	if (!Enabled() || !owner->IsActive())
	{
		return;
	}

	// You are the one neo:
	float4x4 neo = frustum.ViewProjMatrix();
	neo.Inverse();
	
	App->debug_draw->DrawFrustum(neo, float3(0.8f, 0.8f, 1.0f));
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

camera_projection_mode ComponentCamera::GetProjectionMode() const
{
	return projection_mode;
}

bool ComponentCamera::GetShouldRender() const
{
	return should_render;
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

void ComponentCamera::SetShouldRender(bool new_should_render)
{
	should_render = new_should_render;
}

void ComponentCamera::LookAt(const math::float3& direction)
{
	owner->Transform()->LookAt(direction.Normalized());
}

bool ComponentCamera::DoesOBBHavePointInsideFrustum(const math::OBB& obb) const
{
	if (obb.Intersects(frustum))
	{
		return true;
	}

	// Get planes of frustum:
	math::Plane planes[6];
	frustum.GetPlanes(planes);

	// Get points of the obb:
	math::float3 points[8];
	obb.GetCornerPoints(points);

	// Iterate through all the points of obb:
	for (size_t i = 0; i < 8; ++i)
	{
		// Start as assuming the point is inside the
		// frustum:
		bool is_inside_frustum = true;

		// Iterate through all the planes of frustum:
		for (size_t j = 0; j < 6; ++j)
		{
			// Check if the point lies on the plane or in the negative
			// side of the plane:
			if (!planes[j].ProjectToNegativeHalf(points[i]).Equals(points[i]))
			{
				// If it's not on the plane or in the negative side of the 
				// plane, the point is not inside the frustum as it's
				// on the positive side of the plane.
				// Set is_inside_frustum to false, break out of loop
				// and stop checking for current point:
				is_inside_frustum = false;

				break;
			}
		}

		// If the point is inside the frustum, return true as obb
		// has a point inside the frustum:
		if (is_inside_frustum)
		{
			return true;
		}
	}

	// TODO: Refactor this as we have duplicate code.
	// Maybe have a function that compares points
	// with planes in Util.h.

	// Check if we have a false negative, but this time
	// test frustum corner points against obb planes:
	// Get points of frustum to points:
	frustum.GetCornerPoints(points);
	// Get planes of obb to planes:
	obb.GetFacePlanes(planes);

	// Iterate through all the points of frustum:
	for (size_t i = 0; i < 8; ++i)
	{
		// Start as assuming the frustum point is inside the
		// obb:
		bool is_inside_obb = true;

		// Iterate through all the planes of obb:
		for (size_t j = 0; j < 6; ++j)
		{
			// Check if the point lies on the plane or in the negative
			// side of the plane:
			if (!planes[j].ProjectToNegativeHalf(points[i]).Equals(points[i]))
			{
				// If it's not on the plane or in the negative side of the 
				// plane, the point is not inside the obb as it's
				// on the positive side of the plane.
				// Set is_inside_obb to false, break out of loop
				// and stop checking for current point:
				is_inside_obb = false;

				break;
			}
		}

		// If the point is inside the obb, return true as frustum
		// has a point inside the obb:
		if (is_inside_obb)
		{
			return true;
		}
	}
	
	// Return false as if the execution comes to this line,
	// it means there is no point of obb that lies inside
	// the frustum ore vice versa:
	return false;
}

void ComponentCamera::DrawInspectorContent()
{
	bool enabled_editor = Enabled();
	if (ImGui::Checkbox("Enabled", &enabled_editor))
	{
		enabled_editor ? Enable() : Disable();
	}

	ImGui::NewLine();

	float fov_editor = math::RadToDeg(GetHorizontalFOV());
	float near_plane_editor = GetNearPlaneDistance();
	float far_plane_editor = GetFarPlaneDistance();

	if (ImGui::DragFloat("Horizontal FOV", &fov_editor, 0.1f, 45.0f, 120.0f, "%.3f"))
	{
		SetHorizontalFOV(math::DegToRad(fov_editor));
	}
	if (ImGui::DragFloat("Near", &near_plane_editor,0.1f, 0.001f, 10000.0f, "%.3f"))
	{
		SetNearPlaneDistance(near_plane_editor);
	}
	if (ImGui::DragFloat("Far", &far_plane_editor, 0.1f, 0.001f, 10000.0f, "%.3f"))
	{
		SetFarPlaneDistance(far_plane_editor);
	}

	// NOTE: For now we only have support for perspective cameras on editor, therefore, 
	// they cannot create an orthographic camera.
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

/// <summary>
/// x and y should be between -1 and 1.
/// -1, 1   1, 1
/// -1,-1   1,-1
/// </summary>
/// <param name="x"></param>
/// <param name="y"></param>
/// <returns></returns>
math::LineSegment ComponentCamera::GenerateRayFromNormalizedPositions(float x, float y)
{
	return frustum.UnProjectLineSegment(x, y);
}

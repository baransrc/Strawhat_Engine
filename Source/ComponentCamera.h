#pragma once

#include "Component.h"
#include "Event.h"

#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Geometry/OBB.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Geometry/LineSegment.h"

/// <summary>
/// Enum that defines the projection mode of ComponentCamera
/// </summary>
enum class camera_projection_mode
{
	PERSPECTIVE,
	ORTHOGRAPHIC,
};

class ComponentCamera : public Component
{
private:
	/// <summary>
	/// Frustum of the camera.
	/// </summary>
	math::Frustum frustum;

	/// <summary>
	/// Projection matrix defined by the frustum of the camera.
	/// </summary>
	math::float4x4 projection_matrix;

	/// <summary>
	/// View matrix that is the inverse of the global matrix of
	/// this ComponentCamera's owner Entity's ComponentTransform.
	/// </summary>
	math::float4x4 view_matrix;

	/// <summary>
	/// Projection mode of the camera.
	/// </summary>
	camera_projection_mode projection_mode;

	/// <summary>
	/// Flag that should be set to true by any functionality
	/// that may cause a need to calculate the projection matrix
	/// of the camera.
	/// </summary>
	bool should_calculate_projection_matrix;

	/// <summary>
	/// Flag that sets this camera as the main camera. If true,
	/// camera will perform culling on the scene it belongs to.
	/// </summary>
	bool is_main_camera;

	/// <summary>
	/// If this is not true, camera does not set the shader's projection
	/// and view matrices.
	/// </summary>
	bool should_render;

	/// <summary>
	/// Listeners that is subscribed to the component changed events
	/// of the owner Entity.
	/// </summary>
	EventListener<component_type> component_changed_event_listener;

public:
	ComponentCamera();
	~ComponentCamera() override;

	/// <summary>
	/// Initializes this ComponentCamera with it's owner and
	/// initializes frustum, calculates matrices, and subscribes
	/// to the component changed events of the owner Entity.
	/// </summary>
	/// <param name="new_owner">Entity to be this ComponentCamera's owner</param>
	void Initialize(Entity* new_owner) override;

	/// <summary>
	/// Called on each PreUpdate of owner Entity.
	/// Calculates the Projection matrix if needed, and sets the related uniforms
	/// of the shader.
	/// </summary>
	void PreUpdate() override;

	/// <summary>
	/// Draws the frustum gizmo. Called each DrawGizmos of owner Entity.
	/// </summary>
	void DrawGizmo() override;

	/// <returns>
	/// The type of this ComponentCamera.
	///	</returns>
	component_type Type() const override;

	/// <returns>
	/// The view matrix of this ComponentCamera.
	///	</returns>
	const math::float4x4& GetViewMatrix() const;

	/// <returns>
	/// The projection matrix of this ComponentCamera.
	///	</returns>
	const math::float4x4& GetProjectionMatrix() const;

	/// <returns>
	/// The vertical field of view of this ComponentCamera.
	///	</returns>
	float GetVerticalFOV() const;

	/// <returns>
	/// The horizontal field of view of this ComponentCamera.
	///	</returns>
	float GetHorizontalFOV() const;

	/// <returns>
	/// The aspect ratio of this ComponentCamera.
	///	</returns>
	float GetAspectRatio() const;

	/// <returns>
	/// The orthographic width of this ComponentCamera.
	/// May be a gibberish value if the camera is not 
	/// orthographic.
	///	</returns>
	float GetOrthographicWidth() const;

	/// <returns>
	/// The orthographic height of this ComponentCamera.
	/// May be a gibberish value if the camera is not 
	/// orthographic.
	///	</returns>
	float GetOrthographicHeight() const;

	/// <returns>
	/// The far plane distance of this ComponentCamera.
	/// May be a gibberish value if camera is not perspective.
	///	</returns>
	float GetFarPlaneDistance() const;

	/// <returns>
	/// The near plane distance of this ComponentCamera.
	/// May be a gibberish value if camera is not perspective.
	///	</returns>
	float GetNearPlaneDistance() const;

	/// <returns>
	/// The projection mode of this ComponentCamera.
	/// It's either Perspective of Orthographic.
	///	</returns>
	camera_projection_mode GetProjectionMode() const;

	/// <returns>
	/// If this ComponentCamera is set as render camera.
	///	</returns>
	bool GetShouldRender() const;

	/// <summary>
	/// Sets horizontal field of view of this ComponentCamera.
	/// </summary>
	/// <param name="new_horizontal_fov">Value to be set as horizontal field of view.</param>
	void SetHorizontalFOV(float new_horizontal_fov);

	/// <summary>
	/// Sets the aspect ratio of this ComponentCamera.
	/// </summary>
	/// <param name="new_aspect_ratio">Value to be set as the aspect ratio.</param>
	void SetAspectRatio(float new_aspect_ratio);

	/// <summary>
	/// Sets the far plane distance of this ComponentCamera.
	/// This value is only used if the camera is set as perspective.
	/// </summary>
	/// <param name="new_far_plane_distance">Value to be set as the far plane distance.</param>
	void SetFarPlaneDistance(float new_far_plane_distance);

	/// <summary>
	/// Sets the near plane distance of this ComponentCamera.
	/// This value is only used if the camera is set as perspective.
	/// </summary>
	/// <param name="new_near_plane_distance">Value to be set as the near plane distance.</param>
	void SetNearPlaneDistance(float new_near_plane_distance);

	/// <summary>
	/// Sets both far and near plane distances of this ComponentCamera.
	/// Serves as a convenience to the people who wants to change both
	/// plane distances without causing a projection matrix calculation
	/// two times in the inner frustum object.
	/// </summary>
	/// <param name="new_near_plane_distance">Value to be set as the near plane distance.</param>
	/// <param name="new_far_plane_distance">Value to be set as the far plane distance.</param>
	void SetPlaneDistances(float new_near_plane_distance, float new_far_plane_distance);

	/// <summary>
	/// Sets this ComponentCamera as either main camera or non-main camera.
	/// </summary>
	/// <param name="new_is_main_camera">True if the camera should be main camera, false otherwise.</param>
	void SetIsMainCamera(bool new_is_main_camera);

	/// <summary>
	/// Sets the camera as perspective along with the provided horizontal field of
	/// view and aspect ratio.
	/// </summary>
	/// <param name="new_horizontal_fov">Value to be set as horizontal field of view.</param>
	/// <param name="new_aspect_ratio">Value to be set as the aspect ratio.</param>
	void SetAsPerspective(float new_horizontal_fov, float new_aspect_ratio);

	/// <summary>
	/// Sets the camera as orthographic along with provided orthographic width and height.
	/// </summary>
	/// <param name="new_orthographic_width">Value to be set as the orthographic width.</param>
	/// <param name="new_orthographic_height">Value to be set as the orthographic height.</param>
	void SetAsOrthographic(float new_orthographic_width, float new_orthographic_height);

	/// <summary>
	/// Sets if the camera should manipulate the uniforms to be sent by camera to the shader on 
	/// each PreUpdate.
	/// </summary>
	/// <param name="new_should_render">True if should render, false otherwise.</param>
	void SetShouldRender(bool new_should_render);

	/// <summary>
	/// Sets camera's rotation to look at the provided direction.
	/// </summary>
	/// <param name="direction">Sirection to be looked at.</param>
	void LookAt(const math::float3& direction);

	/// <summary>
	/// Runs an intersection test between obb and frustum.
	/// </summary>
	/// <param name="obb">Obb to be tested against this ComponentCamera's frustum.</param>
	/// <returns>True if obb intersects with frustum, false otherwise.</returns>
	bool DoesOBBIntersectFrustum(const math::OBB& obb) const;

	/// <summary>
	/// Creates a ray shot from the x and y position on the near plane 
	/// of the frustum.
	/// x and y should be between -1 and 1.
	/// Positions are mapped as follows:
	/// -1, 1   1, 1
	/// -1,-1   1,-1
	/// </summary>
	/// <param name="x">Position on the x axis.</param>
	/// <param name="y">Position on the y axis.</param>
	/// <returns></returns>
	math::LineSegment GenerateRayFromNormalizedPositions(float x, float y) const;

protected:
	/// <summary>
	/// Draws the inspector content of this ComponentCamera.
	/// </summary>
	void DrawInspectorContent() override;

private:
	/// <summary>
	/// Calculates the projection matrix, both cached one and
	/// the frustum's.
	/// </summary>
	void CalculateProjectionMatrix();

	/// <summary>
	/// Updates values that is effected by ComponentTransform of
	/// the owner Entity.
	/// </summary>
	void UpdateTransformVariables();

	/// <summary>
	/// Called when the event that component_changed_event_listener is 
	/// subscribed to gets invoked.
	/// </summary>
	/// <param name="type">Type of component that is changed.</param>
	void HandleComponentChanged(component_type type);
};
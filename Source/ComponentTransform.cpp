#include "ComponentTransform.h"
#include "Entity.h"

ComponentTransform::ComponentTransform() :
position(math::float3::zero),
position_local(math::float3::zero),
scale(math::float3::one),
scale_local(math::float3::one),
rotation_euler(math::float3::zero),
rotation_euler_local(math::float3::zero),
rotation(math::Quat()),
rotation_local(math::Quat()),
matrix(math::float4x4::identity),
right(math::float3::zero),
up(math::float3::zero),
front(math::float3::zero)
{
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);

	SetEulerRotation(math::float3::zero);
}

void ComponentTransform::Update()
{
}

void ComponentTransform::DrawGizmo()
{
}

component_type ComponentTransform::Type() const
{
	return component_type::TRANSFORM;
}

const math::float3& ComponentTransform::GetPosition() const
{
	return position;
}

const math::float3& ComponentTransform::GetScale() const
{
	return scale;
}

const math::float3& ComponentTransform::GetEulerRotation() const
{
	return rotation_euler;
}

const math::float3& ComponentTransform::GetLocalPosition() const
{
	return position_local;
}

const math::float3& ComponentTransform::GetLocalScale() const
{
	return scale_local;
}

const math::float3& ComponentTransform::GetLocalEulerRotation() const
{
	return rotation_euler_local;
}

const math::Quat& ComponentTransform::GetRotation() const
{
	return rotation;
}

const math::Quat& ComponentTransform::GetLocalRotation() const
{
	return rotation_local;
}

const math::float4x4& ComponentTransform::GetMatrix() const
{
	return matrix;
}

const math::float3& ComponentTransform::GetRight() const
{
	return right;
}

const math::float3& ComponentTransform::GetUp() const
{
	return up;
}

const math::float3& ComponentTransform::GetFront() const
{
	return front;
}

void ComponentTransform::SetPosition(const math::float3& new_position)
{
	position = new_position;

	CalculateLocalPositionFromPosition();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::SetScale(const math::float3& new_scale)
{
	scale = new_scale;

	CalculateLocalScaleFromScale();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::SetEulerRotation(const math::float3& new_rotation_euler)
{
	SetRotation(math::Quat::FromEulerXYZ(math::DegToRad(new_rotation_euler.x), math::DegToRad(new_rotation_euler.y), math::DegToRad(new_rotation_euler.z)));
}

void ComponentTransform::SetRotation(const math::Quat& new_rotation)
{
	rotation = new_rotation;
	rotation_euler = rotation.ToEulerXYZ().Mul(RadToDeg(1.0f));

	CalculateLocalRotationFromRotation();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::SetLocalPosition(const math::float3& new_position_local)
{
	position_local = new_position_local;

	CalculatePositionFromLocalPosition();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::SetLocalScale(const math::float3& new_scale_local)
{
	scale_local = new_scale_local;

	CalculateScaleFromLocalScale();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::SetLocalEulerRotation(const math::float3& new_rotation_euler_local)
{
	SetLocalRotation(
		math::Quat::FromEulerXYZ(
			math::DegToRad(new_rotation_euler_local.x), 
			math::DegToRad(new_rotation_euler_local.y), 
			math::DegToRad(new_rotation_euler_local.z)
		)
	);
}

void ComponentTransform::SetLocalRotation(const math::Quat& new_rotation_local)
{
	rotation_local = new_rotation_local;
	rotation_euler_local = rotation_local.ToEulerXYZ().Mul(RadToDeg(1.0f));

	CalculateRotationFromLocalRotation();

	UpdateTransformOfHierarchy(false);
}

void ComponentTransform::DrawInspectorContent()
{
	math::float3 position_local_editor = position_local;
	math::float3 rotation_local_editor = rotation_euler_local;
	math::float3 scale_local_editor = scale_local;

	if (ImGui::DragFloat3("Local Position", position_local_editor.ptr(), 1.0f, -inf, inf)) {
		SetLocalPosition(position_local_editor);
	}
	if (ImGui::DragFloat3("Local Rotation", rotation_local_editor.ptr(), 1.0f, -360.f, 360.f)) {
		SetLocalEulerRotation(rotation_local_editor);
	}
	if (ImGui::DragFloat3("Local Scale", scale_local_editor.ptr(), 1.0f, 0.0001f, inf, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		SetLocalScale(scale_local_editor);
	}

	ImGui::NewLine();

	math::float3 position_editor = position;
	math::float3 rotation_editor = rotation_euler;
	math::float3 scale_editor = scale;

	if (ImGui::DragFloat3("Position", position_editor.ptr(), 1.0f, -inf, inf)) {
		SetPosition(position_editor);
	}
	if (ImGui::DragFloat3("Rotation", rotation_editor.ptr(), 1.0f, -360.f, 360.f)) {
		SetEulerRotation(rotation_editor);
	}
	if (ImGui::DragFloat3("Scale", scale_editor.ptr(), 1.0f, 0.0001f, inf, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		SetScale(scale_editor);
	}
}

void ComponentTransform::CalculatePositionFromLocalPosition()
{
	Entity* owner_parent = owner->Parent();

	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::float3 position_parent = owner_parent->Transform()->GetPosition();

		position = position_parent + position_local;
	}
	else
	{
		position = position_local;
	}
}

void ComponentTransform::CalculateLocalPositionFromPosition()
{
	Entity* owner_parent = owner->Parent();

	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::float3 position_parent = owner_parent->Transform()->GetPosition();

		position_local = position - position_parent;
	}
	else
	{
		position_local = position;
	}
}

void ComponentTransform::CalculateRotationFromLocalRotation()
{
	rotation = rotation_local;

	Entity* owner_parent = owner->Parent();
	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::Quat parent_rotation = owner_parent->Transform()->GetRotation();

		rotation = parent_rotation.Mul(rotation_local);
	}

	rotation_euler = rotation.ToEulerXYZ().Mul(RadToDeg(1.0f));
}

void ComponentTransform::CalculateLocalRotationFromRotation()
{
	rotation_local = rotation;

	Entity* owner_parent = owner->Parent();
	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::Quat parent_rotation_inverse = owner_parent->Transform()->GetRotation().Inverted();

		rotation_local = parent_rotation_inverse.Mul(rotation);
	}

	rotation_euler_local = rotation_local.ToEulerXYZ().Mul(RadToDeg(1.0f));
}

void ComponentTransform::CalculateScaleFromLocalScale()
{
	Entity* owner_parent = owner->Parent();
	
	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::float3 scale_parent = owner_parent->Transform()->GetScale();

		scale = scale_parent.Mul(scale_local);
	}
	else
	{
		scale = scale_local;
	}
}

void ComponentTransform::CalculateLocalScaleFromScale()
{
	Entity* owner_parent = owner->Parent();

	if (owner_parent != nullptr)
	{
		// As all entities have transform component by
		// default no need to check for nullptr.
		math::float3 scale_parent = owner_parent->Transform()->GetScale();

		scale_local = scale.Div(scale_parent);
	}
	else
	{
		scale_local = scale;
	}
}

void ComponentTransform::CalculateMatrix(bool marked_as_dirty_by_parent)
{
	// If this transform was marked as dirty by parent entity's
	// transform, recalculate position, rotation & scale
	// using local_position, local_rotation & local_scale along
	// with parent's position, rotation & scale:
	if (marked_as_dirty_by_parent)
	{
		CalculatePositionFromLocalPosition();
		CalculateRotationFromLocalRotation();
		CalculateScaleFromLocalScale();
	}

	math::float4x4 translation_matrix = math::float4x4::identity;
	math::float4x4 rotation_matrix = math::float4x4::identity;
	math::float4x4 scaling_matrix = math::float4x4::identity;

	// Setup the translation matrix:
	translation_matrix[0][3] = -position.x;
	translation_matrix[1][3] = -position.y;
	translation_matrix[2][3] = -position.z;

	// Setup the rotation matrix:
	rotation_matrix = rotation * rotation_matrix;

	// Setup the scaling matrix:
	scaling_matrix[0][0] = scale.x;
	scaling_matrix[1][1] = scale.y;
	scaling_matrix[2][2] = scale.z;

	// Calculate transform matrix:
	matrix = rotation_matrix * translation_matrix * scaling_matrix;

	// Set the front up and right:
	front = -float3(rotation_matrix[2][0], rotation_matrix[2][1], rotation_matrix[2][2]);
	up = float3(rotation_matrix[1][0], rotation_matrix[1][1], rotation_matrix[1][2]);
	right = float3(rotation_matrix[0][0], rotation_matrix[0][1], rotation_matrix[0][2]);
}

void ComponentTransform::UpdateTransformOfHierarchy(bool marked_as_dirty_by_parent)
{
	CalculateMatrix(marked_as_dirty_by_parent);

	// TODO: Put transform_changed_event here.

	const std::vector<Entity*>& owner_children = owner->GetChildren();

	for (Entity* owner_child : owner_children)
	{
		owner_child->Transform()->UpdateTransformOfHierarchy(true);
	}
}
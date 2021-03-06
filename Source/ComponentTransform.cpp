#include "ComponentTransform.h"

#include "ModuleDebugDraw.h"
#include "ModuleCamera.h"
#include "ComponentCamera.h"
#include "Application.h"

#include "MATH_GEO_LIB/Math/float3x3.h"
#include "MATH_GEO_LIB/Math/TransformOps.h"

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
#define DEG_TO_RAD (PI / 180.0)
#define RAD_TO_DEG (180.0 / PI)

ComponentTransform::ComponentTransform() :
	Component(),
	position(math::float3::zero),
	position_local(math::float3::zero),
	scale(math::float3::one),
	scale_local(math::float3::one),
	rotation_euler(math::float3::zero),
	rotation_euler_local(math::float3::zero),
	rotation(math::Quat::identity),
	rotation_local(math::Quat::identity),
	matrix(math::float4x4::identity),
	matrix_local(math::float4x4::identity),
	right(math::float3::unitX),
	up(math::float3::unitY),
	front(math::float3::unitZ)
{
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Initialize(Entity* new_owner)
{
	Component::Initialize(new_owner);

	SetEulerRotation(math::float3::zero);

	owner_hierarchy_changed_event_listener = 
		EventListener<entity_operation>
		(
			std::bind
			(
				&ComponentTransform::HandleOwnerHierarchyChanged, 
				this, 
				std::placeholders::_1
			)
		);
	// Listen to owner hierarchy changed event:
	owner->GetHierarchyChangedEvent()->
		AddListener(&owner_hierarchy_changed_event_listener);

}

void ComponentTransform::Update()
{
	
}

void ComponentTransform::DrawGizmo()
{
	//App->debug_draw->DrawArrow(position, position + position.Length() * front, float3(1.0f, 0.0f, 1.0f), 0.1f);
	//App->debug_draw->DrawArrow(position, position + 20.0f * front, float3(0.0f, 0.0f, 1.0f), 0.1f);
	//App->debug_draw->DrawArrow(position, position + 20.0f * right, float3(1.0f, 0.0f, 0.0f), 0.1f);
	//App->debug_draw->DrawArrow(position, position + 20.0f * up, float3(0.0f, 1.0f, 0.0f), 0.1f);
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

const math::float4x4 ComponentTransform::GetLocalMatrix() const
{
	return matrix_local;
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

const math::float3& ComponentTransform::GetDirection() const
{
	return (-front).Normalized();
}

math::Quat ComponentTransform::SimulateLookAt(const math::float3& direction)
{
	math::float3 right_temp = float3::unitY.Cross(direction).Normalized();
	
	math::float3 up_temp = direction.Cross(right_temp).Normalized();

	math::Quat orientation = math::Quat::LookAt(-float3::unitZ, direction, float3::unitY, float3::unitY);

	return orientation;
}

void ComponentTransform::SetPosition(const math::float3& new_position)
{
	position = new_position;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL);
}

void ComponentTransform::SetScale(const math::float3& new_scale)
{
	scale = new_scale;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL);
}

void ComponentTransform::SetEulerRotation(const math::float3& new_rotation_euler)
{
	math::float3 delta = (new_rotation_euler - rotation_euler) * DEG_TO_RAD;
	math::Quat rotation_amount = math::Quat::FromEulerXYZ(delta.x, delta.y, delta.z).Normalized();

	rotation = rotation_amount * (rotation);
	rotation_euler = new_rotation_euler;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL);
}

void ComponentTransform::SetRotation(const math::Quat& new_rotation)
{
	rotation = new_rotation.Normalized();
	rotation_euler = rotation.ToEulerXYZ().Mul(RAD_TO_DEG);

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL);
}

void ComponentTransform::SetLocalPosition(const math::float3& new_position_local)
{
	position_local = new_position_local;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL);
}

void ComponentTransform::SetLocalScale(const math::float3& new_scale_local)
{
	scale_local = new_scale_local;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL);
}

void ComponentTransform::SetLocalEulerRotation(const math::float3& new_rotation_euler_local)
{
	math::float3 delta = (new_rotation_euler_local - rotation_euler_local) * DEG_TO_RAD;
	math::Quat rotation_amount = math::Quat::FromEulerXYZ(delta.x, delta.y, delta.z);

	rotation_local = rotation_amount * (rotation_local);
	rotation_euler_local = new_rotation_euler_local;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL);
}

void ComponentTransform::SetLocalRotation(const math::Quat& new_rotation_local)
{
	rotation_local = new_rotation_local;
	rotation_euler_local = rotation_local.ToEulerXYZ() * RAD_TO_DEG;

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL);
}

void ComponentTransform::Rotate(const math::Quat& rotate_by)
{
	SetRotation(rotate_by * rotation);
}

void ComponentTransform::LookAt(const math::float3& direction)
{
	math::float3 right_temp = float3::unitY.Cross(direction).Normalized();
	math::float3 up_temp = direction.Cross(right_temp).Normalized();

	math::Quat orientation = math::Quat::LookAt(-float3::unitZ, direction, float3::unitY, float3::unitY);

	SetRotation(SimulateLookAt(direction));
}

void ComponentTransform::DrawInspectorContent()
{
	// This controls the sensiti vity of sliders inside the transform editor:
	static float variable_sensitivity = 1.0f;

	math::float3 position_local_editor = position_local;
	math::float3 rotation_local_editor = rotation_euler_local;
	math::float3 scale_local_editor = scale_local;

	if (ImGui::DragFloat3("Local Position", position_local_editor.ptr(), variable_sensitivity, -inf, inf)) {
		SetLocalPosition(position_local_editor);
	}
	if (ImGui::DragFloat3("Local Rotation", rotation_local_editor.ptr(), variable_sensitivity, 0.0f, 360.f)) {
		SetLocalEulerRotation(rotation_local_editor);
	}
	if (ImGui::DragFloat3("Local Scale", scale_local_editor.ptr(), variable_sensitivity, 0.0001f, inf, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		SetLocalScale(scale_local_editor);
	}

	ImGui::NewLine();

	math::float3 position_editor = position;
	math::float3 rotation_editor = rotation_euler;
	math::float3 scale_editor = scale;

	if (ImGui::DragFloat3("Position", position_editor.ptr(), variable_sensitivity, -inf, inf)) {
		SetPosition(position_editor);
	}
	if (ImGui::DragFloat3("Rotation", rotation_editor.ptr(), variable_sensitivity, 0.0f, 360.f)) {
		SetEulerRotation(rotation_editor);
	}
	if (ImGui::DragFloat3("Scale", scale_editor.ptr(), variable_sensitivity, 0.0001f, inf, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		SetScale(scale_editor);
	}

	ImGui::NewLine();

	if (ImGui::Button("LookAt 0,0,0"))
	{
		LookAt((position - float3::zero).Normalized());
	}

	ImGui::NewLine();

	ImGui::DragFloat("Sensitivity", &variable_sensitivity, 0.001f, 0.000001f, inf, "%.3f");
}

void ComponentTransform::HandleOwnerHierarchyChanged(entity_operation operation)
{
	if (operation != entity_operation::PARENT_CHANGED)
	{
		return;
	} 

	UpdateTransformOfHierarchy(transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL);
}

void ComponentTransform::CalculateTransform(transform_matrix_calculation_mode mode)
{
	Entity* owners_parent = owner->Parent();

	switch (mode)
	{
		case transform_matrix_calculation_mode::LOCAL_FROM_GLOBAL:
		{
			matrix = float4x4::FromTRS(position, rotation, scale);

			matrix_local = owners_parent == nullptr ? 
				matrix : 
				owners_parent->Transform()->GetMatrix().Inverted() * matrix;

			matrix_local.Decompose(position_local, rotation_local, scale_local);

			rotation_euler_local = rotation_local.ToEulerXYZ() * RAD_TO_DEG;
		}
		break;

		case transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL:
		{
			matrix_local = float4x4::FromTRS(position_local, rotation_local, scale_local);

			matrix = owners_parent == nullptr ? 
				matrix_local : 
				owners_parent->Transform()->GetMatrix() * matrix_local;

			matrix.Decompose(position, rotation, scale);

			rotation_euler = rotation.ToEulerXYZ() * RAD_TO_DEG;
		}
		break;
	}

	right = matrix.WorldX();
	up = matrix.WorldY();
	front = matrix.WorldZ();
}

void ComponentTransform::UpdateTransformOfHierarchy(transform_matrix_calculation_mode mode)
{
	CalculateTransform(mode);

	// NOTE(Baran): This event must be invoked after new matrix is calculated.
	owner->InvokeComponentsChangedEvents(Type());

	const std::vector<Entity*>& owner_children = owner->GetChildren();

	// As their parent transform is changed only, only updating
	// the global transform matrix of children will suffice.
	for (Entity* owner_child : owner_children)
	{
		owner_child->Transform()->UpdateTransformOfHierarchy(transform_matrix_calculation_mode::GLOBAL_FROM_LOCAL);
	}
}
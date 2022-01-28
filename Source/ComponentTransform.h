#pragma once
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/Quat.h"
#include "MATH_GEO_LIB/Math/float4x4.h"

#include "Component.h"

enum class transform_matrix_calculation_mode
{
	LOCAL_FROM_GLOBAL,
	GLOBAL_FROM_LOCAL
};

class ComponentTransform : public Component
{
private:
	math::float3	position;
	math::float3	position_local;
	math::float3	scale;
	math::float3	scale_local;
	math::float3	rotation_euler;
	math::float3	rotation_euler_local;
	math::Quat		rotation;
	math::Quat		rotation_local;
	math::float3	right;
	math::float3	up;
	math::float3	front;
	math::float4x4	matrix;
	math::float4x4  matrix_local;

public:
	ComponentTransform();
	~ComponentTransform() override;

	void Initialize(Entity* new_owner) override;
	void Update() override;
	void DrawGizmo() override;
	
	component_type Type() const override;
	
	const math::float3& GetPosition() const;
	const math::float3& GetScale() const;
	const math::float3& GetEulerRotation() const;
	const math::float3& GetLocalPosition() const;
	const math::float3& GetLocalScale() const;
	const math::float3& GetLocalEulerRotation() const;
	const math::Quat& GetRotation() const;
	const math::Quat& GetLocalRotation() const;
	const math::float4x4& GetMatrix() const;
	const math::float4x4 GetLocalMatrix() const;
	const math::float3& GetRight() const;
	const math::float3& GetUp() const;
	const math::float3& GetFront() const;
	const math::float3& GetDirection() const;
	static math::Quat SimulateLookAt(const math::float3& direction);

	void SetPosition(const math::float3& new_position);
	void SetScale(const math::float3& new_scale);
	void SetEulerRotation(const math::float3& new_rotation_euler);
	void SetRotation(const math::Quat& new_rotation);
	void SetLocalPosition(const math::float3& new_position_local);
	void SetLocalScale(const math::float3& new_scale_local);
	void SetLocalEulerRotation(const math::float3& new_rotation_euler_local);
	void SetLocalRotation(const math::Quat& new_rotation_local);
	void Rotate(const math::Quat& rotate_by);
	void LookAt(const math::float3& direction);

protected:
	void DrawInspectorContent() override;

private: 
	void CalculateTransform(transform_matrix_calculation_mode mode);
	void UpdateTransformOfHierarchy(transform_matrix_calculation_mode mode);
};
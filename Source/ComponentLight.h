#pragma once

#include "Component.h"
#include "ComponentLightType.h"

#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/Quat.h"

class ComponentLight : public Component
{
private:
	/// <summary>
	/// The type of the light. Either spot, directional or 
	/// point.
	/// </summary>
	light_type current_light_type;

	/// <summary>
	/// Radius of the point light.
	/// </summary>
	float radius;

	/// <summary>
	/// Inner angle of the spotlight light.
	/// </summary>
	float inner_angle;

	/// <summary>
	/// Outer angle of the spotlight light.
	/// </summary>
	float outer_angle;

	/// <summary>
	/// Intensity of the light. Applies to all types of
	/// lights.
	/// </summary>
	float intensity;

	/// <summary>
	/// Color of the light. Applies to all types of lights.
	/// </summary>
	math::float3 color;

public:
	ComponentLight();
	~ComponentLight() override;

	/// <returns>
	/// Component Type of this ComponentLight. Which
	/// is LIGHT.
	/// </returns>
	component_type Type() const override;
	
	/// <summary>
	/// Initializes this ComponentLight with it's owner
	/// Entity.
	/// </summary>
	/// <param name="new_owner">Entity to be set as the owner of this ComponentLight.</param>
	void Initialize(Entity* new_owner) override;

	/// <summary>
	/// Called on each Update of owner Entity.
	/// </summary>
	void Update() override;

	/// <summary>
	/// Called on each DrawGizmos of owner Entity.
	/// Right now, draws a gizmo only for Spotlight.
	/// </summary>
	void DrawGizmo() override;

	/// <summary>
	/// Sets the light_type of this ComponentLight.
	/// </summary>
	/// <param name="new_light_type">Light type to be set as the light_type.</param>
	void SetLightType(light_type new_light_type);

	/// <summary>
	/// Sets the radius of the directional light.
	/// </summary>
	/// <param name="new_radius">Value to be set as the radius of directional light.</param>
	void SetRadius(float new_radius);

	/// <summary>
	/// Sets the inner angle of the spotlight.
	/// </summary>
	/// <param name="new_inner_angle">Value to be set as the inner angle of spotlight.</param>
	void SetInnerAngle(float new_inner_angle);

	/// <summary>
	/// Sets the outer angle of the spotlight.
	/// </summary>
	/// <param name="new_outer_angle">Value to be set as outer angle of spotlight.</param>
	void SetOuterAngle(float new_outer_angle);

	/// <summary>
	/// Sets the intensity of the light. Applies to all types of lights.
	/// </summary>
	/// <param name="new_intensity">Values to be set as the intesity of this ComponentLight.</param>
	void SetIntensity(float new_intensity);

	/// <summary>
	/// Sets the color of this ComponentLight. Applies to all types of lights.
	/// </summary>
	/// <param name="new_color">Value to be set as the color.</param>
	void SetColor(const math::float3& new_color);

	/// <returns> 
	/// Type of light modeled by this ComponentLight.
	/// </returns>
	light_type GetLightType() const;

	/// <returns> 
	/// Color of the light modeled by this ComponentLight.
	/// </returns>
	const math::float3& GetColor() const;

	/// <returns> 
	/// Radius of the point light modeled by this ComponentLight.
	/// </returns>
	const float GetRadius() const;

	/// <returns> 
	/// Inner angle of the spot light modeled by this ComponentLight.
	/// </returns>
	const float GetInnerAngle() const;

	/// <returns> 
	/// Outer angle of the spot light modeled by this ComponentLight.
	/// </returns>
	const float GetOuterAngle() const;

	/// <returns> 
	/// Intensity of the light modeled by this ComponentLight.
	/// </returns>
	const float GetIntensity() const;

protected:
	/// <summary>
	/// Called on Component::DrawInspector. 
	/// </summary>
	void DrawInspectorContent() override;

private:
	/// <summary>
	/// Sets the uniforms that will be sent to the shader
	/// according to the type of light.
	/// </summary>
	void SetUniforms();

	/// <summary>
	/// Sends the point light uniforms to the shader.
	/// </summary>
	void SetUniformsPointLight();

	/// <summary>
	/// Sends the directional light uniforms to the shader.
	/// </summary>
	void SetUniformsDirectionalLight();

	/// <summary>
	/// Sends the spotlight uniforms to the shader.
	/// </summary>
	void SetUniformsSpotLight();
};
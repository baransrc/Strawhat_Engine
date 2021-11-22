#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"
#include "MATH_GEO_LIB/Math/float4x4.h"
#include "MATH_GEO_LIB/Math/float3.h"

class ModuleCamera : public Module
{
private:
	Frustum frustum;

	// Matrices:
	float4x4 model_matrix;
	float4x4 view_matrix;
	float4x4 projection_matrix;

	// Properties:
	bool is_perspective = true;
	float3 position;
	float aspect_ratio;
	float fov_horizontal;
	float fov_vertical;
	float orthographic_width;
	float orthographic_height;
	float near_plane;
	float far_plane;
public:
	ModuleCamera();
	~ModuleCamera();
	
	bool Init();
	bool CleanUp();

	float4x4 GetModelMatrix() const { return model_matrix; };
	float4x4 GetViewMatrix() const { return view_matrix; };
	float4x4 GetProjectionMatrix() const { return projection_matrix; };

	
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
};


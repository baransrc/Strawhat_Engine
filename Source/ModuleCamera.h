#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Geometry/Frustum.h"

class ModuleCamera : public Module
{
private:
	Frustum frustum;
public:
	ModuleCamera();
	~ModuleCamera();
	
	bool Init();
	bool CleanUp();
	
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
};


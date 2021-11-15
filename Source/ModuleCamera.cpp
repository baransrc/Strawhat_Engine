#include "ModuleCamera.h"
#include "Globals.h"
#include "MathGeoLib.h"

ModuleCamera::ModuleCamera()
{
}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	return true;
}

bool ModuleCamera::CleanUp()
{
	return true;
}

update_status ModuleCamera::PreUpdate()
{
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

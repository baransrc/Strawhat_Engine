#pragma once
#include "Module.h"

class ModuleEditor : public Module
{
public:
	bool Init();
	bool CleanUp();
	update_status Update();
private:
	void InitializeDearImGui();
	void UninitializeDearImGui();
};


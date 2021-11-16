#pragma once
#include "Module.h"

class ModuleEditor : public Module
{
private:
	bool show_demo_window = true;
	bool show_about_window = false;
	bool show_console_window = true;
	char* license_buffer = nullptr;

public:
	ModuleEditor();
	~ModuleEditor();

	bool Init();
	bool CleanUp();
	
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	void DrawMainMenuBar();
	void DrawAboutWindow();
	void DrawConsoleWindow();

private:
	void InitializeDearImGui();
	void UninitializeDearImGui();
};


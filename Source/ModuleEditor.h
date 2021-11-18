#pragma once
#include "Module.h"

class ModuleEditor : public Module
{
private:
	bool show_demo_window = true;
	bool show_about_window = false;
	bool show_console_window = false;
	bool show_render_exercise_texture_info_window = false;
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
	void DrawRenderExerciseTextureInfoWindow();

private:
	void InitializeDearImGui();
	void UninitializeDearImGui();
};


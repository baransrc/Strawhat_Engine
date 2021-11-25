#pragma once
#include "Module.h"
#include <vector>

class ModuleEditor : public Module
{
private:
	bool show_demo_window = true;
	bool show_about_window = false;
	bool show_console_window = false;
	bool show_render_exercise_texture_info_window = false;
	bool show_performance_window = false;
	char* license_buffer = nullptr;
	std::vector<float> ms_data;
	std::vector<float> fps_data;

public:
	ModuleEditor();
	~ModuleEditor() override;

	bool Init();
	bool CleanUp();

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	void DrawMainMenuBar();
	void DrawAboutWindow();
	void DrawConsoleWindow();
	void DrawRenderExerciseTextureInfoWindow();
	void DrawPerformanceWindow();

private:
	void InitializeDearImGui();
	void UninitializeDearImGui();
};
#pragma once

#include "Module.h"

#include "ImGuizmo.h"

#include <vector>

class Entity;

class ModuleEditor : public Module
{
private:
	// TODO: Store these flags in a bool array
	// create a WindowFlagId enum to store their
	// index and access them through that:
	bool show_demo_window = true;
	bool show_about_window = false;
	bool show_console_window = false;
	bool show_performance_window = false;
	bool show_module_settings_window = false;
	bool show_exit_popup = false;
	bool should_exit_application = false;
	bool should_draw_inspector_window = true;
	bool should_draw_hierarchy_window = true;

	ImGuizmo::OPERATION current_imguizmo_operation;
	ImGuizmo::MODE current_imguizmo_transform_mode;

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
	void DrawExitPopup();
	void DrawAboutWindow();
	void DrawConsoleWindow();
	void DrawPerformanceWindow();
	void DrawInspector();
	void DrawImGuizmoModeWindow();
	void DrawImGuizmo();
	void DrawHierarchy();
	void DrawModuleSettings();

private:
	void SwitchImGuizmoOperationModeWithKeyboard();
	void TransformSelectedEntityWithImGuizmo();
	void InitializeDearImGui();
	void UninitializeDearImGui();
};
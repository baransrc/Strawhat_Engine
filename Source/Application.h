#pragma once

#include "Module.h"
#include "Globals.h"

#include <vector>

class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleRenderExercise;
class ModuleShaderProgram;
class ModuleRenderExercise;
class ModuleEditor;
class ModuleTexture;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleSceneManager;

class Application
{
public:

	Application();
	~Application();

	bool Init();
	bool Start();
	update_status Update();
	bool CleanUp();

	const char* GetWorkingDirectory() const { return working_directory; };

public:
	ModuleRender* renderer = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleShaderProgram* shader_program = nullptr;
	ModuleSceneManager* scene_manager = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleTexture* texture = nullptr;
	ModuleCamera* camera = nullptr;
	ModuleDebugDraw* debug_draw = nullptr;

private:
	char* working_directory = nullptr;
	std::vector<Module*> modules;
};

extern Application* App;
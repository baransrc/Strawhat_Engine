#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class Model;
class Entity;

class ModuleRender : public Module
{
public:
	void* context;

private:
	unsigned int viewport_width = SCREEN_WIDTH;
	unsigned int viewport_height = SCREEN_HEIGHT;
	Entity* default_entity;
	Entity* loaded_entity;
	//Model* default_model;
	//Model* model = nullptr;
	float clear_color[4] = {0.0f, 0.0f, 0.0f, 1.0f};

public:
	ModuleRender();
	~ModuleRender() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);
	void OnDropFile(char* file_directory);

	Entity* GetLoadedModel() const { return loaded_entity == nullptr ? default_entity : loaded_entity; };
	//const Model* GetLoadedModel() const { return model == nullptr ? default_model : model; };

	float GetRequiredAxisTriadLength() const;
	
	const void* GetContext() const { return context; };

	void OnEditor();
	void OnPerformanceWindow() const;
private:
	void InitializeModel(char* file_directory);
	void InitializeOpenGL();
	void InitializeGLEW();
	void LogHardware();
	void InitializeRenderPipelineOptions();
};
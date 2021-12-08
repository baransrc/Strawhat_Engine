#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class Model;

class ModuleRender : public Module
{
public:
	void* context;

private:
	unsigned int viewport_width = SCREEN_WIDTH;
	unsigned int viewport_height = SCREEN_HEIGHT;
	Model* default_model;
	Model* model = nullptr;

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


	const Model* GetLoadedModel() const { return model == nullptr ? default_model : model; };

	float GetRequiredAxisTriadLength() const;
	
	const void* GetContext() const { return context; };

private:
	void InitializeModel(char* file_directory);
	void InitializeOpenGL();
	void InitializeGLEW();
	void LogHardware();
	void InitializeRenderPipelineOptions();
};

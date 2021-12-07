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
	Model* model;

public:
	ModuleRender();
	~ModuleRender() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	const Model* GetLoadedModel() const { return model; };
	
	const void* GetContext() const { return context; };

private:
	void InitializeOpenGL();
	void InitializeGLEW();
	void LogHardware();
	void InitializeRenderPipelineOptions();
};

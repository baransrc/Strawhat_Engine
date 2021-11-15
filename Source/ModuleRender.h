#pragma once
#include "Module.h"
#include "Globals.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
private:
	void* context;
	unsigned int vertex_array_object = 0;
	unsigned int viewport_width = SCREEN_WIDTH;
	unsigned int viewport_height = SCREEN_HEIGHT;
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void WindowResized(unsigned width, unsigned height);

	const void* GetContext() const { return context; };

private:
	void InitializeOpenGL();
	void InitializeGLEW();
	void LogHardware();
	void InitializeRenderPipelineOptions();
};

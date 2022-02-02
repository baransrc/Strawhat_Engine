#pragma once
#include "Module.h"
#include "Globals.h"
#include "Event.h"

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
	EventListener<const char*> file_dropped_event_listener;
	EventListener<unsigned int, unsigned int> window_resized_event_listener;
	float clear_color[4] = {0.176f, 0.176f, 0.176f, 1.0f};

public:
	ModuleRender();
	~ModuleRender() override;

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void HandleWindowResized(unsigned int width, unsigned int height);
	float GetRequiredAxisTriadLength() const;
	const void* GetContext() const { return context; };

	void OnEditor();
	void OnPerformanceWindow() const;

private:

	void InitializeOpenGL();
	void InitializeGLEW();
	void LogHardware();
	void InitializeRenderPipelineOptions();
};
#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float2.h"

enum class key_state
{
	IDLE = 0,
	DOWN = 1,
	REPEAT = 2,
	UP = 3,
};

class ModuleInput : public Module
{
public:
	
	ModuleInput();
	~ModuleInput() override;

	bool Init();
	bool CleanUp();
	
	update_status PreUpdate();
	
	bool GetKey(size_t key_code, key_state state);
	float2 GetMousePosition() { return mouse_position; }
	float2 GetMouseDisplacement() { return mouse_displacement; }

private:
	uint32_t window_width = SCREEN_WIDTH;
	uint32_t window_height = SCREEN_HEIGHT;
	float window_width_inverse = 1.0f / (float)window_width;
	float window_height_inverse = 1.0f / (float)window_height;
	const uint8_t* keyboard = NULL;
	key_state* keyboard_state;
	float2 mouse_position = float2::zero;
	float2 mouse_displacement = float2::zero;
};
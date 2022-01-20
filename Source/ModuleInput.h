#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math/float2.h"
#include "Event.h"

constexpr size_t NUM_MOUSE_BUTTONS = 5;

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

	bool GetKey(size_t key_code, key_state state) const;
	bool GetMouseKey(size_t mouse_key_code, key_state state) const;
	float2 GetMousePosition() const { return mouse_position; }
	float2 GetMouseDisplacement() const { return mouse_displacement; }
	float2 GetMouseWheelDisplacement() const { return mouse_wheel_displacement; }
	Event<const char*>* const GetFileDroppedEvent() const { return file_dropped_event; }
	Event<unsigned int, unsigned int>* const GetWindowResizedEvent() const { return window_resized_event; }

private:
	Event<unsigned int, unsigned int>* window_resized_event;
	Event<const char*>* file_dropped_event;
	uint32_t window_width = SCREEN_WIDTH;
	uint32_t window_height = SCREEN_HEIGHT;
	float window_width_inverse = 1.0f / (float)window_width;
	float window_height_inverse = 1.0f / (float)window_height;
	const uint8_t* keyboard = NULL;
	key_state* keyboard_state;
	key_state mouse_buttons_state[NUM_MOUSE_BUTTONS];
	float2 mouse_position = float2::zero;
	float2 mouse_displacement = float2::zero;
	float2 mouse_wheel_displacement = float2::zero;
};
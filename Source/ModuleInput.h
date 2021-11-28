#pragma once
#include "Module.h"
#include "Globals.h"

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

private:
	const uint8_t* keyboard = NULL;
	key_state* keyboard_state;
};
#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleCamera.h"
#include "SDL/include/SDL.h"
#include "imgui_impl_sdl.h"

constexpr size_t NUM_MAX_KEYS = 300;

ModuleInput::ModuleInput()
{
}

// Destructor
ModuleInput::~ModuleInput()
{}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);
    
	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

    // Get Keyboard Array of SDL:
    keyboard = SDL_GetKeyboardState(nullptr);

    // Initialize Keyboard State array with each element as IDLE:
    keyboard_state = new key_state[NUM_MAX_KEYS];
    memset(keyboard_state, static_cast<int>(key_state::IDLE), sizeof(key_state) * NUM_MAX_KEYS);

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate()
{
    SDL_Event sdl_event;
    
    for (size_t i = 0; i < NUM_MAX_KEYS; ++i)
    {
        if (keyboard[i] == 1)
        {
            if (keyboard_state[i] == key_state::IDLE)
            {
                keyboard_state[i] = key_state::DOWN;
            }
            else
            {
                keyboard_state[i] = key_state::REPEAT;
            }
        }
        else
        {
            if (keyboard_state[i] == key_state::REPEAT || keyboard_state[i] == key_state::DOWN)
            {
                keyboard_state[i] = key_state::UP;
            }
            else
            {
                keyboard_state[i] = key_state::IDLE;
            }
        }
    }

    while (SDL_PollEvent(&sdl_event) != 0)
    {
        // Send input events to Dear ImGui:
        ImGui_ImplSDL2_ProcessEvent(&sdl_event);

        switch (sdl_event.type)
        {
            case SDL_QUIT:
            {
                return update_status::UPDATE_STOP;
            }

            case SDL_WINDOWEVENT:
            {
                if (sdl_event.window.event == SDL_WINDOWEVENT_RESIZED || sdl_event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
                {
                    App->renderer->WindowResized(sdl_event.window.data1, sdl_event.window.data2);
                    App->camera->WindowResized(sdl_event.window.data1, sdl_event.window.data2);
                }
            }
            break;
        }
    }
    
    keyboard = SDL_GetKeyboardState(NULL);
    
    return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);

    free(keyboard_state);

	return true;
}

bool ModuleInput::GetKey(size_t key_code, key_state state)
{
    return keyboard_state[key_code] == state;
}

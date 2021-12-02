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

    // Reset Mouse Displacement:
    mouse_displacement = float2::zero;

    // Reset Mouse Scroll Wheel Displacement:
    mouse_wheel_displacement = float2::zero;

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
                    window_width = sdl_event.window.data1;
                    window_height = sdl_event.window.data2;
                    window_width_inverse = 1.0f / (float)max(1, window_width);
                    window_height_inverse = 1.0f / (float)max(1, window_height);

                    App->renderer->WindowResized(window_width, window_height);
                    App->camera->WindowResized(window_width, window_height);
                }
            }
            break;

            case SDL_MOUSEMOTION:
            {
                mouse_displacement.x = sdl_event.motion.xrel;
                mouse_displacement.y = sdl_event.motion.yrel;

                mouse_position.x = sdl_event.motion.x * window_width_inverse;
                mouse_position.y = sdl_event.motion.y * window_height_inverse;
            }
            break;

            case SDL_MOUSEWHEEL:
            {
                mouse_wheel_displacement.x = sdl_event.wheel.x;
                mouse_wheel_displacement.y = sdl_event.wheel.y;
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

bool ModuleInput::GetKey(size_t key_code, key_state state) const
{
    return keyboard_state[key_code] == state;
}

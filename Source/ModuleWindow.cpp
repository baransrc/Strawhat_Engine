#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow() : window_width(0), window_height(0)
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;
    
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		SDL_DisplayMode display_mode;
		SDL_GetCurrentDisplayMode(0, &display_mode);

		window_width = (display_mode.w * 2) / 3;
		window_height = (display_mode.h * 2) / 3;

		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL;
        
		if(FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
        
		if (RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}
        
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, window_width, window_height, flags);
		
		// Initialize window_width and window_height
		SDL_GetWindowSize(window, &window_width, &window_height);
        
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			
			screen_surface = SDL_GetWindowSurface(window);
		}
	}
    
	return ret;
}

update_status ModuleWindow::PreUpdate()
{
	SDL_GetWindowSize(window, &window_width, &window_height);
    
	return update_status::UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");
    
	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}
    
	//Quit SDL subsystems
	SDL_Quit();
	return true;
}


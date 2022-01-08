#include <stdlib.h>
#include "Application.h"
#include "ModuleRender.h"
#include "Globals.h"
#include "Event.h"

#include "SDL/include/SDL.h"
#pragma comment( lib, "SDL/lib/x64/SDL2.lib" )
#pragma comment( lib, "SDL/lib/x64/SDL2main.lib" )

#ifdef _DEBUG
#define MYDEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__)
#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#define new MYDEBUG_NEW
#endif

void DumpLeaks(void)
{
	_CrtDumpMemoryLeaks(); // Show leaks with file and line where allocation was made
}

enum class main_states
{
	MAIN_CREATION,  
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Console* console = NULL;
TimeManager* Time = NULL;
Application* App = NULL;

void SomeFunc1(int a, float b)
{
	LOG("SomeFunc1: %i %f", a, b);
}

void SomeFunc2(int a, float b)
{
	LOG("SomeFunc2: %i %f", a, b);
}

int main(int argc, char ** argv)
{
	atexit(DumpLeaks);
	
	int main_return = EXIT_FAILURE;

	main_states state = main_states::MAIN_CREATION;

	console = new Console();
	Time = new TimeManager();

	Event<int, float> some_event = Event<int, float>();

	EventListener<int, float> some_func_1 = EventListener<int, float>(std::bind(&SomeFunc1, std::placeholders::_1, std::placeholders::_2));
	EventListener<int, float> some_func_2 = EventListener<int, float>(std::bind(&SomeFunc2, std::placeholders::_1, std::placeholders::_2));

	some_event.AddListener(&some_func_1);
	some_event.AddListener(&some_func_2);

	some_event.Invoke(1, 2.333f);

	some_event.RemoveListener(&some_func_2);

	some_event.Invoke(3, 9999.0f);

	while (state != main_states::MAIN_EXIT)
	{
		switch (state)
		{
		case main_states::MAIN_CREATION:
			LOG("Application Creation --------------");
			App = new Application();
			state = main_states::MAIN_START;
			break;

		case main_states::MAIN_START:

			LOG("Application Init --------------");
			if (App->Init() == false)
			{
				LOG("Application Init exits with error -----");
				state = main_states::MAIN_EXIT;
			}
			else
			{
				state = main_states::MAIN_UPDATE;
				LOG("Application Update --------------");
			}

			break;

		case main_states::MAIN_UPDATE:
		{
			Time->Start();

			update_status update_return = App->Update();

			Time->End();

			if (update_return == update_status::UPDATE_ERROR)
			{
				LOG("Application Update exits with error -----");
				state = main_states::MAIN_EXIT;
			}

			if (update_return == update_status::UPDATE_STOP)
				state = main_states::MAIN_FINISH;
		}
		break;

		case main_states::MAIN_FINISH:

			LOG("Application CleanUp --------------");
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----");
			}
			else
				main_return = EXIT_SUCCESS;

			state = main_states::MAIN_EXIT;

			break;

		}

	}

	LOG("Bye :)\n");
	delete App;
	delete Time;
	delete console;
	
	return main_return;
}

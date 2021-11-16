#pragma once
#include <string>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#define CONSOLE_MAX_LINES 256

struct Console
{
public:
	Console() 
	{
	};

	inline void AddLine(const char* new_line)
	{
		// This code below utilizes a sort of clock like behaving array of logs.
		// Before it current_end reaches to the end of the array, CONSOLE_MAX_LINE,
		// it behaves like a normal array and increments only the current_end before
		// adding a new string to the array.

		add_counter = min(++add_counter, CONSOLE_MAX_LINES - 1);

		current_end = (current_end + 1) % CONSOLE_MAX_LINES;
		
		lines_array[current_end].clear();
		lines_array[current_end] = std::string(new_line);

		// Once add_counter reaches CONSOLE_MAX_LINES - 1, in other words all the array is full
		// with logs, and the last added line was the same with the current_start, we increment
		// current_start clockwise (modulo addition), so that oldest added log becomes the first
		// log to be printed in the console.
		if (add_counter == CONSOLE_MAX_LINES - 1 && current_end == current_start)
		{
			current_start = (current_start + 1) % CONSOLE_MAX_LINES;
		}
	}

	inline void ToImGuiText()
	{
		// We define a counter so that we take one turn in the array.
		uint32_t counter = 0;
		// Logs will be printed starting from the oldest log possible.
		uint32_t current_pos = current_start;

		// While one turn is not over:
		while (counter < CONSOLE_MAX_LINES-1)
		{
			// Increment the counter:
			counter++;
			// Increment current_pos clockwise:
			current_pos = (current_pos + 1) % CONSOLE_MAX_LINES ;

			// If current line is empty, do not print it to the console:
			if (lines_array[current_pos].empty())
			{
				continue;
			}

			ImGui::TextWrapped((lines_array[current_pos]).c_str());
			ImGui::TextWrapped("\n");
			ImGui::Separator();
		}
	}

	~Console()
	{
	}

private:
	uint32_t add_counter = 0;
	uint32_t current_start = 0;
	uint32_t current_end = -1;
	std::string lines_array[CONSOLE_MAX_LINES];
};


// This file is included by Globals.h and the console variable is 
// defined inside the main.cpp. With this, all the classes can interact 
// with console. In the future, for example, a file manager class may take
// the logs of the console and write them to a file.
extern Console* console;
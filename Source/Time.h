#pragma once
#include "SDL/include/SDL.h"
#include "DEAR_IMGUI/include/imgui.h"
#include <iostream>
#include <algorithm>

constexpr int TIMER_BUFFER_LENGTH = 60;
constexpr float TIMER_BUFFER_LENGTH_INVERSE = (1.0f / (float)TIMER_BUFFER_LENGTH);
constexpr float DIVIDE_BY_THOUSAND = (1.0f / 1000.0f);
constexpr float FPS_CAP_60 = 16.66666666666667f;

class NormalTimer
{
private:
	uint32_t start_time;
	uint32_t end_time;
	bool is_stopped;

public:
	NormalTimer() : start_time(0), end_time(0), is_stopped(false) {};

	~NormalTimer() {};

	void Start()
	{
		start_time = SDL_GetTicks(); // NOTE: SDL documentation says that this value wraps if the program runs for more than ~49 days. Might use SDL_GetTicks64() instead.
	};

	uint32_t Read()
	{
		if (!is_stopped)
		{
			end_time = SDL_GetTicks(); // NOTE: SDL documentation says that this value wraps if the program runs for more than ~49 days. Might use SDL_GetTicks64() instead.
		}

		return end_time - start_time;
	};

	void Stop()
	{
		is_stopped = true;
	};
};

class PerformanceTimer
{
private:
	uint64_t start_time;
	uint64_t end_time;
	bool is_stopped;

public:
	PerformanceTimer() : start_time(0), end_time(0), is_stopped(false) {};

	~PerformanceTimer() {};

	void Start()
	{
		start_time = SDL_GetPerformanceCounter();
	};

	float Read()
	{
		if (!is_stopped)
		{
			end_time = SDL_GetPerformanceCounter();
		}

		return (end_time - start_time) * 1000.0f / (float)SDL_GetPerformanceFrequency();
	};

	void Stop()
	{
		is_stopped = true;
	};
};

class TimeManager
{
private:
	size_t current_index;
	unsigned int current_frame;
	float frame_times_ms[TIMER_BUFFER_LENGTH];
	float delta_time_ms;
	float fps;
	NormalTimer timer;

public:
	TimeManager() : current_index(-1), current_frame(0), delta_time_ms(0.0f), fps(0.0f), timer()
	{
		std::fill_n(frame_times_ms, TIMER_BUFFER_LENGTH, 0.0f);
	};

	~TimeManager() {};

	void Start()
	{
		timer.Start();
	};

	void End(bool cap_to_60_fps = true)
	{
		delta_time_ms = timer.Read();

		if (cap_to_60_fps)
		{
			float delay_amount = max(0.0f, (FPS_CAP_60 - delta_time_ms));

			delta_time_ms += delay_amount;

			SDL_Delay(delay_amount);
		}
		
		current_index = (current_index + 1) % TIMER_BUFFER_LENGTH;

		frame_times_ms[current_index] = delta_time_ms;
		
		++current_frame;

		fps = CalculateFPS();
	};

	float FPS() const
	{
		return fps;
	};

	float DeltaTime() const
	{
		return delta_time_ms * DIVIDE_BY_THOUSAND;
	};

	float DeltaTimeMs() const
	{
		return delta_time_ms;
	};

private:
	float CalculateFPS()
	{
		float sum = 0.0f;
		for (size_t i = 0; i < TIMER_BUFFER_LENGTH; ++i)
		{
			sum += frame_times_ms[i];
		}

		if (current_frame < TIMER_BUFFER_LENGTH)
		{
			return 1.0f/(sum * DIVIDE_BY_THOUSAND / (float)current_frame);
		}

		return 1.0f / (sum * DIVIDE_BY_THOUSAND * TIMER_BUFFER_LENGTH_INVERSE);
	}
};

extern TimeManager* Time;
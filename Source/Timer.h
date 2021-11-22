#pragma once
#include "SDL/include/SDL.h"
#include <iostream>
#include <algorithm>

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
		start_time = SDL_GetTicks();
	};

	uint32_t Read()
	{
		if (!is_stopped)
		{
			end_time = SDL_GetTicks();
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

	uint64_t Read()
	{
		if (!is_stopped)
		{
			end_time = SDL_GetPerformanceCounter();
		}

		return (end_time - start_time) / 1000;
	};

	void Stop()
	{
		is_stopped = true;
	};
};

// This will be the Time class.
//#define TIMER_BUFFER_LENGTH 60
//class Timer
//{
//private:
//	float frame_times[TIMER_BUFFER_LENGTH];
//	Uint64 start_time;
//	float delta_time;
//	float frames_per_second;
//	size_t current_index;
//public:
//	Timer() : current_index(0), delta_time(0)
//	{
//		std::fill_n(frame_times, TIMER_BUFFER_LENGTH, 0.0f);
//	};
//
//	~Timer();
//
//	void StartTimer() 
//	{
//		start_time = SDL_GetPerformanceCounter();
//	};
//
//	void EndTimer() 
//	{
//		Uint64 end_time = SDL_GetPerformanceCounter();
//		Uint64 delta_ticks = end_time - start_time;
//
//		delta_time = (float)delta_ticks * 1000.0f / (float)SDL_GetPerformanceFrequency();
//		
//		frame_times[current_index] = delta_time;
//		++current_index;
//		
//		frames_per_second = CalculateFramesPerSecond();
//	};
//
//	float GetFramesPerSecond() 
//	{
//		frames_per_second;
//	};
//
//	float GetDeltaTime() const 
//	{
//		return delta_time;
//	};
//private:
//	float CalculateFramesPerSecond()
//	{
//		float sum = 0.0f;
//		for (size_t i = 0; i < TIMER_BUFFER_LENGTH; ++i)
//		{
//			sum += frame_times[i];
//		}
//
//		return sum / TIMER_BUFFER_LENGTH;
//	}
//};
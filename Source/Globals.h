#pragma once
#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define FULLSCREEN false
#define RESIZABLE true
#define RENDERER_CULL_FACE true
#define RENDERER_DEPTH_TEST true
#define RENDERER_SCISSOR_TEST false
#define RENDERER_STENCIL_TEST false
#define VSYNC true
#define TITLE "Strawhat Engine"
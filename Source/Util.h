#ifndef _UTIL_H
#define _UTIL_H

#include "GL/glew.h"
#include "windows.h"
#include "string.h"
#include <direct.h> // _getcwd
#include <iostream>

namespace util
{
	inline void __stdcall OpenGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
	{
		const char* tmp_source = "", * tmp_type = "", * tmp_severity = "";
		switch (source) {
		case GL_DEBUG_SOURCE_API: tmp_source = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: tmp_source = "Window System"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: tmp_source = "Shader Compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: tmp_source = "Third Party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: tmp_source = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: tmp_source = "Other"; break;
		};
		switch (type) {
		case GL_DEBUG_TYPE_ERROR: tmp_type = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: tmp_type = "Deprecated Behaviour"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: tmp_type = "Undefined Behaviour"; break;
		case GL_DEBUG_TYPE_PORTABILITY: tmp_type = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: tmp_type = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER: tmp_type = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: tmp_type = "Push Group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: tmp_type = "Pop Group"; break;
		case GL_DEBUG_TYPE_OTHER: tmp_type = "Other"; break;
		};
		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH: tmp_severity = "high"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: tmp_severity = "medium"; break;
		case GL_DEBUG_SEVERITY_LOW: tmp_severity = "low"; break;
		case GL_DEBUG_SEVERITY_NOTIFICATION: tmp_severity = "notification"; break;
		};
		LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
	}

	// TODO: ModuleShaderProgram must use this function too.
	inline void ReadFile(const char* file_name, char** data)
	{
		FILE* file = nullptr;

		// In unix like systems and linux, r and rb are the same since they have single
		// character for endline, \n, but in windows there are multiple characters for
		// endline and additional b mode maps all those into \n.
		// r stands for read mode.
		fopen_s(&file, file_name, "rb"); // TODO: Get current working directory and find the shader file inside folder.

		if (file)
		{
			// Number of bytes to offset from origin:
			static long file_offset = 0;
			// Size in bytes, of each element to be read (since a char is one byte):
			size_t element_size_bytes = 1;

			// To the end of the file:
			fseek(file, file_offset, SEEK_END);

			// Set size to location of the end of the file:
			int size = ftell(file);

			// Allocate needed amont of memory for file:
			*data = (char*)malloc(size + 1);

			// Go to the start of the file:
			fseek(file, 0, SEEK_SET);

			// Read file and write to data:
			fread(*data, element_size_bytes, size, file);

			// Set end of data to null-character:
			(*data)[size] = 0;

			// Close the file:
			fclose(file);
		}
	}

	inline void GetWorkingDirectory(char** buffer)
	{
		*buffer = (char*)malloc(MAX_PATH);
		_getcwd(*buffer, MAX_PATH);
	}

	inline char* ConcatCStrings(const char* first, const char* second)
	{
		uint32_t first_size = strlen(first);
		uint32_t second_size = strlen(second);
		uint32_t concat_size = first_size + second_size;

		char* concat_string = (char*)malloc(concat_size + 1);

		memcpy(concat_string, first, first_size);
		memcpy(&(concat_string[first_size]), second, second_size);

		concat_string[concat_size] = '\0';

		return concat_string;
	}
}

#endif
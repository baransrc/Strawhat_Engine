#ifndef _UTIL_H
#define _UTIL_H

#include "GL/glew.h"
#include "windows.h"
#include "string.h"
#include <direct.h> // _getcwd
#include <iostream>
#include <fstream>
#include <stdlib.h>

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
		// TODO: Uncomment this, for now it's commented out because if the computer uses nVidia GPU, it logs the same message every single frame, and it's cringe:
		//LOG("<Source:%s> <Type:%s> <Severity:%s> <ID:%d> <Message:%s>\n", tmp_source, tmp_type, tmp_severity, id, message);
	}

	// User is responsible for deallocation.
	// TODO: ModuleShaderProgram must use this function too.
	inline void ReadFile(const char* file_name, char** data)
	{
		FILE* file = nullptr;

		// In unix like systems and linux, r and rb are the same since they have single
		// character for endline, \n, but in windows there are multiple characters for
		// endline and additional b mode maps all those into \n.
		// r stands for read mode.
		fopen_s(&file, file_name, "rb");

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

	inline void AppendToFile(const char* file_name, const char* data)
	{
		FILE* file = nullptr;

		fopen_s(&file, file_name, "a"); 

		if (file)
		{
			fprintf(file, "%s", data);

			// Close the file:
			fclose(file);
		}
	} 

	inline bool RemoveFile(const char* file_name)
	{
		// Return true if successful:
		return remove(file_name) == 0;
	}

	inline void OverwriteFile(const char* file_name, const char* data)
	{
		FILE* file;

		fopen_s(&file, file_name, "w");
		
		if (file)
		{
			fprintf(file, "%s", data);

			// Close the file:
			fclose(file);
		}
	}

	// User is responsible for deallocation.
	inline char* GetStringBetween(char* const source, const char* start_string, const char* end_string) 
	{
		char* start;
		char* end;

		start = strstr(source, start_string);
		end = strstr(source, end_string);

		if (start == nullptr || end == nullptr)
		{
			return nullptr;
		}

		// If function is still executing, the wanted portions of the string is found.

		// Find location in source by pointer substractions:
		size_t start_pos = start - source + strlen(start_string); 
		size_t end_pos = end - source - 1;

		size_t buffer_size = end_pos - start_pos + 1;

		// Allocate char array of needed size:
		char* buffer = (char*)malloc(buffer_size + 1);

		// Copy substring between start_pos and end_pos to buffer:
		memcpy(buffer, &(source[start_pos]), buffer_size);

		buffer[buffer_size] = '\0';

		return buffer;
	}

	// User is responsible for deallocation.
	inline void DeleteStringFromStartToEnd(char** source, const char* start_string, const char* end_string)
	{
		char* start;
		char* end;

		size_t debug_s = strlen("\r\n{start_4}\r\nsomething\r\nsomething more\r\nsomething even more\r\n{end_4}{start_7}\r\nsomething\r\nsomething more\r\nsomething even more\r\n{end_7}");

		start = strstr(*source, start_string);
		end = strstr(*source, end_string);

		if (start == nullptr || end == nullptr)
		{
			return;
		}

		// If function is still executing, the wanted portions of the string is found.

		// Find location in source by pointer substractions:
		size_t start_pos = start - (*source);
		size_t end_pos = end - (*source) + strlen(end_string);

		// Determine sizes of 2 pieces of the source string divided by the string 
		// starting from start_string ending at the end of the end_string:
		size_t size_until_start_pos = start_pos;
		size_t size_after_end_pos = strlen((*source)) - end_pos;
		// Buffer size will be the sum of the length of two pieces:
		size_t buffer_size = size_until_start_pos + size_after_end_pos;

		char* buffer = (char*)malloc(buffer_size + 1);

		// Copy the first part:
		memcpy(buffer, (*source), size_until_start_pos);
		// Copy the second part:
		memcpy(&(buffer[size_until_start_pos]), &((*source)[end_pos]), size_after_end_pos);

		buffer[buffer_size] = '\0';

		free(*source);

		*source = buffer;
	}

	// User is responsible for deallocation.
	inline void GetWorkingDirectory(char** buffer)
	{
		*buffer = (char*)malloc(MAX_PATH);
		_getcwd(*buffer, MAX_PATH);
	}

	// User is responsible for deallocation.
	inline char* ConcatCStrings(const char* first, const char* second)
	{
		size_t first_size = strlen(first);
		size_t second_size = strlen(second);
		size_t concat_size = first_size + second_size;

		char* concat_string = (char*)malloc(concat_size + 1);

		memcpy(concat_string, first, first_size);
		memcpy(&(concat_string[first_size]), second, second_size);

		concat_string[concat_size] = '\0';

		return concat_string;
	}

	inline void OpenLink(const char* link)
	{
		ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
	}
}

#endif
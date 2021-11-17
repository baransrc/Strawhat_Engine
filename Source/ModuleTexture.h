#pragma once
#include "Module.h"
#include "GL/glew.h"

class ModuleTexture : public Module
{
public:
	ModuleTexture();
	~ModuleTexture();

	bool Init();
	bool CleanUp();
	
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

	GLuint LoadTexture(const char* file_name, 
					   GLint min_filter, 
					   GLint mag_filter, 
					   GLint wrap_s, 
					   GLint wrap_t,
					   bool is_rgba,
					   bool generate_mipmap) const;

	void UnloadTexture(GLuint* texture_ptr, int count) const;

private:
	bool InitializeDevIL() const;
};
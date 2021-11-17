#pragma once
#include "Module.h"

class Application;

class ModuleShaderProgram : public Module
{
    public:
	ModuleShaderProgram();
    
	bool Init();

	bool CleanUp();
    
	unsigned int CreateProgram(unsigned int vtx_shader_id, unsigned int frg_shader_id) const;
    
	char* LoadProgramSource(const char* file_name) const;
    
	unsigned int CompileShader(unsigned int type, const char* source) const; // This is not going to make changes on vertex_shader_id or fragment_shader_id, Init will change them.
    
	unsigned int GetProgramId() {return program_id;};

	void Use() const;

	~ModuleShaderProgram();
    
    private:
	unsigned int program_id;
};


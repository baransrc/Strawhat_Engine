#pragma once
#include "Module.h"
#include "MATH_GEO_LIB/Math/float3.h"
#include "MATH_GEO_LIB/Math/float4.h"
#include "MATH_GEO_LIB/Math/float4x4.h"

class Application;

class ModuleShaderProgram : public Module
{
    public:
	ModuleShaderProgram();
	~ModuleShaderProgram() override;
    
	bool Init();
	bool CleanUp();
    
	unsigned int CreateProgram(unsigned int vtx_shader_id, unsigned int frg_shader_id) const;
	unsigned int CompileShader(unsigned int type, const char* source) const; // This is not going to make changes on vertex_shader_id or fragment_shader_id, Init will change them.
	unsigned int GetProgramId() {return program_id;};

	void Use() const;

	void SetUniformVariable(const char* name, int value) const;
	void SetUniformVariable(const char* name, const float3& value) const;
	void SetUniformVariable(const char* name, const float4& value) const;
	void SetUniformVariable(const char* name, const float4x4& value, const bool transpose) const;

    private:
	unsigned int program_id;
};


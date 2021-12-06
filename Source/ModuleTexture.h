#pragma once
#include "Module.h"
#include "GL/glew.h"


class ModuleTexture : public Module
{
private:
	char* runtime_texture_data_file_path;
public:
	ModuleTexture();
	~ModuleTexture() override;

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
					   bool generate_mipmap,
				       bool& loading_successful) const;

	void ConfigureTexture(
		const char* texture_file_name,
		GLuint texture_id,
		bool overwrite_data,
		GLint min_filter,
		GLint mag_filter,
		GLint wrap_s,
		GLint wrap_t,
		bool bind_texture,
		bool is_rgba,
		bool generate_mipmap) const;

	void UnloadTexture(GLuint* texture_ptr) const;

	void GetTextureInfo(GLuint texture_id, char** buffer) const;

private:
	bool InitializeDevIL() const;
	void WriteTextureData(GLuint texture_id, const char* texture_file_name) const;
	void DeleteTextureData(GLuint texture_ptr) const;
};
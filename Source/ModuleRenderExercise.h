#pragma once
#include "Module.h"
#include "GLEW/include/GL/glew.h"

class ModuleRenderExercise : public Module
{
private:
    unsigned int vertex_array_object = 0;
    unsigned int vertex_buffer_object = 0;
    unsigned int element_buffer_object = 0;
    GLuint lena_texture;
    char* lena_texture_data;
    char* lena_texture_file_path;

    // Configurations for Lena texture info:
    int wrap_mode_s = 0;
    int wrap_mode_t = 0;
    int mag_filter = 0;
    int min_filter = 0;
    bool mipmap_enabled = 0;
    bool settings_changed = false;
public:
    ModuleRenderExercise();
    ~ModuleRenderExercise() override;
    
    bool Init();
    bool CleanUp();
	
    update_status PreUpdate();
    update_status Update();
    
    void DrawTextureInfoContent();
private:
    void ApplyTextureConfigChanges();
};
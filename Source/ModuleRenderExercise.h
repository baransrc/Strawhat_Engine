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
public:
    ModuleRenderExercise();
    ~ModuleRenderExercise();
    
    bool Init();
    bool CleanUp();
	
    update_status Update();
    
    void DrawTextureInfoContent() const;
};
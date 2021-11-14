#pragma once
#include "Module.h"
class ModuleRenderExercise : public Module
{
private:
    unsigned int vertex_buffer_object = 0;

public:
    ModuleRenderExercise();
    
    bool Init();

    bool CleanUp();
	
    update_status Update();
    
    ~ModuleRenderExercise();
};
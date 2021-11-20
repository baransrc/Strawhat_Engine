#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleShaderProgram.h"
#include "ModuleRenderExercise.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModuleTexture.h"
#include "ModuleCamera.h"
#include "Util.h"

using namespace std;

Application::Application()
{
    // Get working directory and store it inside working_directory:
    util::GetWorkingDirectory(&working_directory);

	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
    modules.push_back(texture = new ModuleTexture());
	modules.push_back(renderer = new ModuleRender());
    modules.push_back(render_exercise = new ModuleRenderExercise());
    modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
    modules.push_back(shader_program = new ModuleShaderProgram());
    modules.push_back(camera = new ModuleCamera());
}

Application::~Application()
{
	for(list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;
    
	for(list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
    {
		ret = (*it)->Init();
    }
    
	return ret;
}

update_status Application::Update()
{
	update_status ret = update_status::UPDATE_CONTINUE;
    
	for(list<Module*>::iterator it = modules.begin(); 
        it != modules.end() && ret == update_status::UPDATE_CONTINUE; 
        ++it)
    {
        ret = (*it)->PreUpdate();
    }
    
	for(list<Module*>::iterator it = modules.begin(); 
        it != modules.end() && ret == update_status::UPDATE_CONTINUE;
        ++it)
    {
		ret = (*it)->Update();
    }
    
	for(list<Module*>::iterator it = modules.begin(); 
        it != modules.end() && ret == update_status::UPDATE_CONTINUE;
        ++it)
    {
		ret = (*it)->PostUpdate();
    }
    
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
    
	for(list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
    {
        ret = (*it)->CleanUp();
    }

    free(working_directory);
    
	return ret;
}

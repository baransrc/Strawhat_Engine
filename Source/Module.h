#pragma once

#include "Globals.h"

class Application;

class Module
{
    public:

	Module()
	{
	}

	virtual ~Module() = default;
    
	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}
    
	virtual update_status PreUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}
    
	virtual update_status Update()
	{
		return update_status::UPDATE_CONTINUE;
	}
    
	virtual update_status PostUpdate()
	{
		return update_status::UPDATE_CONTINUE;
	}
    
	virtual bool CleanUp() 
	{ 
		return true; 
	}
};

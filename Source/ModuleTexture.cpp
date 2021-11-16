#include "ModuleTexture.h"
#include "Globals.h"
#include "IL/il.h"

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
    LOG("DevIL - IL version: %i", ilGetInteger(IL_VERSION_NUM));

    return true;
}

bool ModuleTexture::CleanUp()
{
    return true;
}

update_status ModuleTexture::PreUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

update_status ModuleTexture::Update()
{
    return update_status::UPDATE_CONTINUE;
}

update_status ModuleTexture::PostUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

void ModuleTexture::InitializeDevIL()
{
    
}

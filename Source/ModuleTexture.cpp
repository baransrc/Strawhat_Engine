#include "ModuleTexture.h"
#include "Globals.h"
#include "DEVIL/include/IL/il.h"
#include "DEVIL/include/IL/ilu.h"

#define ERROR_TEXTURE "\\Textures\\error_texture.jpg"

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
    bool init_success = true;

    // Initialize DevIL, if initialization returns false, halt program:
    init_success = init_success && InitializeDevIL();

    return init_success;
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

GLuint ModuleTexture::LoadTexture(const char* file_name, 
                                  GLint min_filter, 
                                  GLint mag_filter, 
                                  GLint wrap_s, 
                                  GLint wrap_t,
                                  bool is_rgba,
                                  bool generate_mipmap) const
{
    ILuint image_id;
    ILboolean load_success;
    GLuint texture_id;

    // Generate one image:
    ilGenImages(1, &image_id);

    // Bind image name:
    ilBindImage(image_id);
    // Try loading the image with name file_name:
    load_success = ilLoadImage(file_name);

    // If image could not be loaded, load error texture image instead:
    if (load_success == IL_FALSE)
    {
        LOG("Texture with filename \"%s\" could not be loaded, loading error texture instead.", file_name);
        // Load error texture:
        ilLoadImage(ERROR_TEXTURE);
        // Interpret error texture as rgb:
        is_rgba = false;
    }

    // Convert every color component into unsigned byte:
    ilConvertImage(is_rgba ? IL_RGBA : IL_RGB, IL_UNSIGNED_BYTE);

    iluRotate(180.0f);

    // Generate texture id:
    glGenTextures(1, &texture_id);
    // Bind texture id:
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Configure Magnification filter:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    // Configure Minification filter:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    // Specify texture parameters:
    glTexImage2D
    (
        GL_TEXTURE_2D,                 // Target texture
        0,                             // Level of detail number
        ilGetInteger(IL_IMAGE_BPP),    // Number of color components in the texture, il defines it as bytes per pixel
        ilGetInteger(IL_IMAGE_WIDTH),  // Width of texture image
        ilGetInteger(IL_IMAGE_HEIGHT), // Height of texture image
        0,                             // Border of texture image, in docs of Khronos, it says this must be zero
        ilGetInteger(IL_IMAGE_FORMAT), // Format of the image 
        GL_UNSIGNED_BYTE,              // Data type of pixel data
        ilGetData()                    // Pointer to the image data in memory
    );

 

    // Generate mipmap if requested:
    if (generate_mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Since image data is already copied to gpu as texture data, 
    // release the memory used by image data:
    ilDeleteImages(1, &image_id);
    
    return texture_id;
}

bool ModuleTexture::InitializeDevIL() const
{
    if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
    {
        return false;
    }
    
    LOG("DevIL - IL version: %i", ilGetInteger(IL_VERSION_NUM));

    // Initialize DevIL IL:
    ilInit();

    // Initialize DevIL ILU:
    iluInit();

    return true;
}

void ModuleTexture::UnloadTexture(GLuint* texture_ptr, int count) const
{
    glDeleteTextures(count, texture_ptr);
}

#include "Application.h"
#include "ModuleTexture.h"
#include "Globals.h"
#include "Util.h"
#include "DEVIL/include/IL/il.h"
#include "DEVIL/include/IL/ilu.h"

constexpr const char* ERROR_TEXTURE = "\\Textures\\error_texture.jpg";
constexpr const char* RUNTIME_TEXTURE_DATA_FILE = "\\Textures\\RUNTIME_TEXTURE.DATA";

ModuleTexture::ModuleTexture()
{
}

ModuleTexture::~ModuleTexture()
{
}

bool ModuleTexture::Init()
{
    bool init_success = true;

    // Initialize runtime texture data file path:
    runtime_texture_data_file_path = util::ConcatCStrings(App->GetWorkingDirectory(), RUNTIME_TEXTURE_DATA_FILE);

    // Initialize DevIL, if initialization returns false, halt program:
    init_success = init_success && InitializeDevIL();

    return init_success;
}

bool ModuleTexture::CleanUp()
{
    util::RemoveFile(runtime_texture_data_file_path);

    free(runtime_texture_data_file_path);

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

void ModuleTexture::ConfigureTexture(
    const char* texture_file_name,
    GLuint texture_id,
    bool overwrite_data,
    GLint min_filter,
    GLint mag_filter,
    GLint wrap_s,
    GLint wrap_t,
    bool bind_texture,
    bool is_rgba,
    bool generate_mipmap) const
{
    if (bind_texture)
    {
        // Bind texture id:
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }
    // Configure Magnification filter:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
    // Configure Minification filter:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    // Generate mipmap if requested:
    if (generate_mipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    if (overwrite_data)
    {
        DeleteTextureData(texture_id);
        WriteTextureData(texture_id, texture_file_name);
    }
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

    //iluRotate(180.0f);

    // Generate texture id:
    glGenTextures(1, &texture_id);
    // Bind texture id:
    glBindTexture(GL_TEXTURE_2D, texture_id);
    // Configure texture:
    ConfigureTexture(
        file_name,
        texture_id,
        false, // NOTE: Since data is just being created for the first time, no need to overwrite it in the runtime file.
        min_filter,
        mag_filter,
        wrap_s,
        wrap_t,
        false, // NOTE: Since we already bind the texture, no need to bind it again. Texture is binded outside to see we are binded the texture explicitly. 
        is_rgba,
        generate_mipmap);

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

    // Write Texture Data to file:
    WriteTextureData(texture_id, file_name);

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

void ModuleTexture::WriteTextureData(GLuint texture_id, const char* texture_file_name) const
{
    // Get Texture Data:
    ILinfo texture_data;
    iluGetImageInfo(&texture_data);
    
    char texture_data_string[512];

    // TODO: Add switch statements for all possible formats defined in il.h,
    // right now, as a clever solution this only writes extension of the file.

    std::string format(texture_file_name);
    size_t dot_index = format.find('.');
    format = format.substr(dot_index+1);

    sprintf
    (
        texture_data_string, 
        TEXTURE_DATA_FORMAT, 
        texture_id,
        texture_file_name, 
        format.c_str(),
        texture_data.Width,
        texture_data.Height,
        texture_data.Depth,
        texture_id
    );


    util::AppendToFile(runtime_texture_data_file_path, texture_data_string);
}

void ModuleTexture::DeleteTextureData(GLuint texture_ptr) const
{
    char* file_buffer;

    util::ReadFile(runtime_texture_data_file_path, &file_buffer);

    // Max unsigned for x64 is : 18446744073709551615, therefore 21 chars are enough.
    // If we ever have x128 or something, this function needs to be tweaked, I guess.
    // for {start_} we have 7, so 32 bytes are more than enough.
    char start_string[32];
    char end_string[32];

    sprintf(start_string, "{start_%lu}", texture_ptr);
    sprintf(end_string, "{end_%lu}", texture_ptr);

    util::DeleteStringFromStartToEnd(&file_buffer, start_string, end_string);

    util::OverwriteFile(runtime_texture_data_file_path, file_buffer);

    free(file_buffer);
}

void ModuleTexture::UnloadTexture(GLuint* texture_ptr) const
{
    DeleteTextureData(*texture_ptr);

    glDeleteTextures(1, texture_ptr);
}

// User must deallocate buffer memory.
void ModuleTexture::GetTextureInfo(GLuint texture_id, char** buffer) const
{
    char* file_buffer;

    util::ReadFile(runtime_texture_data_file_path, &file_buffer);

    // Max unsigned for x64 is: 18446744073709551615, therefore 21 chars are enough.
    // If we ever have x128 or something, this function needs to be tweaked, I guess.
    // for {start_} we have 7, so 32 bytes are more than enough.
    char start_string[32];
    char end_string[32];

    sprintf(start_string, "{start_%lu}", texture_id);
    sprintf(end_string, "{end_%lu}", texture_id);

    *buffer = util::GetStringBetween(file_buffer, start_string, end_string);

    free(file_buffer);
}

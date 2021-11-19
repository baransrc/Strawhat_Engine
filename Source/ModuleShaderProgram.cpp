#include <algorithm>
#include "ModuleShaderProgram.h"
#include "GL/glew.h"

constexpr const char* VERTEX_SHADER_PATH = "vertex.glsl";
constexpr const char* FRAGMENT_SHADER_PATH = "fragment.glsl";

ModuleShaderProgram::ModuleShaderProgram()
{
}

bool ModuleShaderProgram::Init()
{
    bool return_flag = true;

    // Load shaders from files and compile them:
    char* vertex_shader_buffer = LoadProgramSource(VERTEX_SHADER_PATH);
    char* fragment_shader_buffer = LoadProgramSource(FRAGMENT_SHADER_PATH);
    
    unsigned int vertex_shader_id = CompileShader(GL_VERTEX_SHADER, vertex_shader_buffer);
    unsigned int fragment_shader_id = CompileShader(GL_FRAGMENT_SHADER, fragment_shader_buffer);

    free(vertex_shader_buffer);
    free(fragment_shader_buffer);

    // Link shaders and create program:
    program_id = CreateProgram(vertex_shader_id, fragment_shader_id);

    // Use the created program:
    glUseProgram(program_id);

    // Delete shaders since they are linked into the program and they are not needed anymore:
    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);

    return true;
}

bool ModuleShaderProgram::CleanUp()
{
    // Delete shader program:
    glDeleteProgram(program_id);

    return true;
}

unsigned int ModuleShaderProgram::CreateProgram(unsigned int vtx_shader_id, unsigned int frg_shader_id) const
{
    unsigned int shader_program_id;
    // Create a shader program object and assign it's id to shader_program_id:
    shader_program_id = glCreateProgram(); 

    // Attach vertex and fragment shaders to the shader program with shader_program_id:
    glAttachShader(shader_program_id, vtx_shader_id);
    glAttachShader(shader_program_id, frg_shader_id);

    // Link the attached shaders into program:
    glLinkProgram(shader_program_id);

    int linking_success = GL_FALSE;

    // Get linking status and store inside linking_success:
    glGetProgramiv(shader_program_id, GL_LINK_STATUS, &linking_success);

    // If linking is not successful log errors to the console:
    if (linking_success == GL_FALSE)
    {
        int linking_log_length = 0;
        glGetProgramiv(shader_program_id, GL_INFO_LOG_LENGTH, &linking_log_length);

        if (linking_log_length > 0)
        {
            int written_length = 0;
            char* linking_log = (char*) malloc(linking_log_length);

            glGetProgramInfoLog(shader_program_id, linking_log_length, &written_length, linking_log);

            LOG("Shader Program Linking Log: %s", linking_log);

            free(linking_log);
        }
       
    }

    return shader_program_id;
}

char* ModuleShaderProgram::LoadProgramSource(const char* file_name) const
{
    char* data = nullptr;
    FILE* file = nullptr;

    // In unix like systems and linux, r and rb are the same since they have single
    // character for endline, \n, but in windows there are multiple characters for
    // endline and additional b mode maps all those into \n.
    // r stands for read mode.
    fopen_s(&file, file_name, "rb"); // TODO: Get current working directory and find the shader file inside folder.

    if (file)
    {
        // Number of bytes to offset from origin:
        static long file_offset = 0;
        // Size in bytes, of each element to be read (since a char is one byte):
        size_t element_size_bytes = 1;

        // To the end of the file:
        fseek(file, file_offset, SEEK_END);

        // Set size to location of the end of the file:
        int size = ftell(file);

        // Allocate needed amont of memory for file:
        data = (char*) malloc(size+1);

        // Go to the start of the file:
        fseek(file, 0, SEEK_SET);

        // Read file and write to data:
        fread(data, element_size_bytes, size, file);

        // Set end of data to null-character:
        data[size] = 0;

        // Close the file:
        fclose(file);
    }

    LOG("\nLoaded Program:\n------\n%s\n------\n", data);

    return data;
}

unsigned int ModuleShaderProgram::CompileShader(unsigned int type, const char* source) const
{
    // Create shader with given id, either GL_VERTEX_SHADER or GL_FRAGMENT_SHADER:
    unsigned int shader_id = glCreateShader(type);

    static int num_shader_strings = 1;

    // Set the source code in the shader object with shader_id:
    glShaderSource(shader_id, num_shader_strings, &source, NULL); // NULL => Each string is assumed to be null terminated.

    // Compile source code string inside shader object with shader_id:
    glCompileShader(shader_id);

    int compilation_success = GL_FALSE;

    // Get compilation result of shader_id & store it inside compilation_success:
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compilation_success); // glGetShaderiv is used to return a parameter from a shader object with given id.

    if (compilation_success == GL_FALSE)
    {
        // Get compilation log&errors: 
        int compilation_log_length = 0;
        glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &compilation_log_length);

        if (compilation_log_length > 0)
        {
            int written_length = 0;
            char* compilation_log = (char*) malloc(compilation_log_length); 
            glGetShaderInfoLog(shader_id, compilation_log_length, &written_length, compilation_log);
            
            // Write the logs to the console:
            LOG("Shader Compilation Log: %s", compilation_log);

            free(compilation_log);
        }

    }

    return shader_id;
}

void ModuleShaderProgram::Use() const
{
    glUseProgram(program_id);
}

void ModuleShaderProgram::SetUniformVariable(const char* name, int value) const
{
    glUniform1i(glGetUniformLocation(program_id, name), value);
}

ModuleShaderProgram::~ModuleShaderProgram()
{
}

#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec2 vertex_texture_coordinate;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 model_matrix;

out vec3 world_normal;
out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_texture_coordinate;

void main()
{
    world_normal = transpose(inverse(mat3(projection_matrix)))*vertex_normal;
    fragment_position = vec3(model_matrix * vec4(vertex_position, 1.0));
    fragment_normal = transpose(inverse(mat3(model_matrix))) * vertex_normal;
    fragment_texture_coordinate = vertex_texture_coordinate;
    gl_Position = projection_matrix * view_matrix * model_matrix * vec4(vertex_position, 1.0);
}
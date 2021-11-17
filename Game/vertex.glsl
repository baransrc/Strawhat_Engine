#version 330 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_color;
layout (location = 2) in vec2 vertex_texture_coordinate;

out vec3 interpolated_color;
out vec2 interpolated_texture_coordinate;

void main()
{
    gl_Position = vec4(vertex_position, 1.0);
    interpolated_color = vertex_color;
    interpolated_texture_coordinate = vertex_texture_coordinate;
}
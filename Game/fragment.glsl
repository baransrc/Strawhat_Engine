#version 330 core
out vec4 frag_color;

in vec3 interpolated_color;
in vec2 interpolated_texture_coordinate;

uniform sampler2D input_texture;

void main()
{
    // frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    frag_color = texture(input_texture, interpolated_texture_coordinate);
} 
#version 330 core
out vec4 frag_color;

in vec3 interpolated_color;
in vec2 interpolated_texture_coordinate;

uniform sampler2D input_texture;

uniform vec3 directional_light;
uniform vec3 directional_color;
uniform vec3 diffuse_color;
uniform vec3 ambient_color;

in vec3 world_normal;

void main()
{
    //frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //frag_color = texture(input_texture, interpolated_texture_coordinate);
    vec3 ambient_color = vec3 (0.2,0.2,0.2);
    vec3 diffuse_color = texture2D(input_texture, interpolated_texture_coordinate).rgb;
    vec3 N  = normalize(world_normal);
    vec3 L = normalize(directional_light);

    float NdotL = -dot(N,L);

    if(NdotL >= 0.0)
    {
        frag_color = vec4(diffuse_color*NdotL, 1.0);
    } 
    else
    {
        frag_color = vec4(ambient_color*diffuse_color, 1.0);
    }
} 
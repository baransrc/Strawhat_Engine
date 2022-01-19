#version 330 core
out vec4 frag_color;

struct Light {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
}; 


in vec3 interpolated_colorNORMALS;
in vec2 interpolated_texture_coordinate;
in vec3 fragment_position;

uniform sampler2D input_texture;

uniform vec3 directional_light;
uniform vec3 directional_color;
uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform Light light;

in vec3 world_normal;

void main()
{
    //frag_color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    //frag_color = texture(input_texture, interpolated_texture_coordinate);
   /* vec3 ambient_color = vec3 (0.2,0.2,0.2);
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
    }*/

    vec3 lightDir = normalize(light.position - fragment_position);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = ambient_color * texture(input_texture, interpolated_texture_coordinate).rgb;
        
        // diffuse 
        vec3 norm = normalize(interpolated_colorNORMALS);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(input_texture, interpolated_texture_coordinate).rgb;  
        
        // specular
        vec3 viewDir = normalize(light.position - fragment_position);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float shininess = 32.0f;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light.specular * spec * texture(input_texture, interpolated_texture_coordinate).rgb;  
        
        // attenuation
        float distance    = length(light.position - fragment_position);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

        ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;   
            
        vec3 result = ambient + diffuse + specular;
        frag_color = vec4(result, 1.0);
    }
    else 
    {
        // else, use ambient light so scene isn't completely dark outside the spotlight.
        frag_color = vec4(light.ambient * texture(input_texture, interpolated_texture_coordinate).rgb, 1.0);
    }

} 
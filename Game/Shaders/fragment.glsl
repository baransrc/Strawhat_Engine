#version 330 core

#define PI 3.141597

out vec4 frag_color;

struct Light {
    vec3  position;
    vec3  direction;
    float radius;
    float intensity;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
}; 

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D occlusion;
    sampler2D normalmap;
    vec4 color;
};


in vec3 interpolated_colorNORMALS;
in vec2 interpolated_texture_coordinate;
in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_texture_coordinate;

uniform float shininess;
uniform vec3 camera_position;

uniform sampler2D input_texture;

uniform vec3 directional_light;
uniform vec3 directional_color;
//uniform vec3 diffuse_color;
uniform vec3 ambient_color;
uniform Light light;
uniform Material material;

in vec3 world_normal;

vec3 SchlickFresnel(const vec3 f0, float cos_theta)
{
    return f0 + (vec3(1.0) - f0) * pow(1.0 - cos_theta, 5.0);
}

vec3 PBRDirectional()
{
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(-light.direction);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);
    //float shininess = 25.0;
    float intensity = 1.0;
    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, shininess);
    vec3 PRBDirectional = light.diffuse * (step1 + step2) * NdotL * intensity;

    return PRBDirectional;
}

vec3 PBRPoint()
{
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(fragment_position - light.position);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);
    //float shininess = 25.0;

    // attenuation
    float light_distance    = length(light.position - fragment_position);
    float attenuation = max(pow(max(1 - pow(light_distance/light.radius, 4), 0.0), 2.0), 0.0) / ((light_distance * light_distance) + 1);

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, shininess);
    vec3 PBRPoint = light.diffuse * (step1 + step2) * NdotL * attenuation * light.intensity;

    return PBRPoint;
}

vec3 PBRSpot()
{
    vec3 lightDir = normalize(light.position - fragment_position);
    
    // check if lighting is inside the spotlight cone
    float theta = dot(lightDir, normalize(-light.direction)); 
    
    if(theta < light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
    {    
        // ambient
        vec3 ambient = light.ambient * texture(material.diffuse, fragment_texture_coordinate).rgb;
        
        // diffuse 
        vec3 norm = normalize(fragment_normal);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, fragment_texture_coordinate).rgb;  
        
        // specular
        vec3 viewDir = normalize(light.position - fragment_position);
        vec3 reflectDir = reflect(-lightDir, norm);  
        float shininess = 32.0f;
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
        vec3 specular = light.specular * spec * texture(material.diffuse, fragment_texture_coordinate).rgb;  
        
        // attenuation
        float light_distance    = length(light.position - fragment_position);
        float attenuation = 1.0f / (light.constant + light.linear * light_distance + light.quadratic * (light_distance * light_distance));    

        //ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
        diffuse   *= attenuation;
        specular *= attenuation;

        return diffuse+specular;
       }
        else 
        {
            // else, use ambient light so scene isn't completely dark outside the spotlight.
            return vec3(light.ambient * texture(material.diffuse, interpolated_texture_coordinate).rgb);
        }
   
}

void main()
{
    //vec3 PBR = PBRDirectional();
    //vec3 PBR = PBRPoint();
    vec3 PBR = PBRSpot();
    
    PBR += light.ambient*texture(material.diffuse, fragment_texture_coordinate).rgb;

    frag_color = vec4(PBR, 1.0);
      //frag_color = vec4(texture(material.specular, fragment_texture_coordinate).rgb, texture(material.specular, fragment_texture_coordinate).a);
//    if(NdotL >= 0.0)
//    {
//        frag_color = vec4(PBR, 1.0);
//    } 
//    else
//    {
//        frag_color = vec4(light.ambient*texture(material.diffuse, fragment_texture_coordinate).rgb, 1.0);
//    }

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
//
//    vec3 lightDir = normalize(light.position - fragment_position);
//    
//    // check if lighting is inside the spotlight cone
//    float theta = dot(lightDir, normalize(-light.direction)); 
    
//    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.
//    {    
//        // ambient
//        vec3 ambient = light.ambient * texture(material.diffuse, interpolated_texture_coordinate).rgb;
//        
//        // diffuse 
//        vec3 norm = normalize(interpolated_colorNORMALS);
//        float diff = max(dot(norm, lightDir), 0.0);
//        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, interpolated_texture_coordinate).rgb;  
//        
//        // specular
//        vec3 viewDir = normalize(light.position - fragment_position);
//        vec3 reflectDir = reflect(-lightDir, norm);  
//        float shininess = 32.0f;
//        float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
//        vec3 specular = light.specular * spec * texture(material.diffuse, interpolated_texture_coordinate).rgb;  
//        
//        // attenuation
//        float distance    = length(light.position - fragment_position);
//        float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
//
//        //ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
//        //diffuse   *= attenuation;
//        //specular *= attenuation;   
//            
//        vec3 result = ambient + diffuse /*+ specular*/;
//        frag_color = vec4(result, 1.0);
//    }
//    else 
//    {
//        // else, use ambient light so scene isn't completely dark outside the spotlight.
//        frag_color = vec4(light.ambient * texture(material.diffuse, interpolated_texture_coordinate).rgb, 1.0);
//        //frag_color = vec4((0.2, 0.2, 0.2) * texture(input_texture, interpolated_texture_coordinate).rgb, 1.0);
//    }
    //frag_color = vec4(0.3, 1.0, 0.3, 1.0);
    //frag_color = vec4(light.ambient * texture(material.diffuse, interpolated_texture_coordinate).rgb, 1.0);
} 
#version 330 core

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

out vec4 frag_color;

struct Light {
    vec3  position;
    vec3  direction;
    float radius;
    float intensity;
    float inner;
    float outer;
  
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

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, shininess);
    vec3 PRBDirectional = light.diffuse * (step1 + step2) * NdotL * light.intensity;

    return PRBDirectional;
}

vec3 PBRPoint()
{
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(light.position - fragment_position);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);

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
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(light.position - fragment_position);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);
    
    //Cone attenuation
    float cone_attenuation = 0.0;
    float C = dot(-L, normalize(light.direction));
    float cos_inner = cos(light.inner);
    if (C > cos_inner) cone_attenuation = 1.0;
    float cos_outer = cos(light.outer);
    if (C > cos_outer) cone_attenuation = (C - cos_outer)/(cos_inner - cos_outer);


    // attenuation
    float light_distance = dot(-L, normalize(light.direction));
    float attenuation = max(pow(max(1 - pow(light_distance/light.radius, 4), 0.0), 2.0), 0.0) / ((light_distance * light_distance) + 1);

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, shininess);
    vec3 PBRSpot = light.diffuse * (step1 + step2) * NdotL * (attenuation * cone_attenuation) * light.intensity;

    return PBRSpot;
 
}

void main()
{
    //vec3 PBR = PBRDirectional();
    vec3 PBR = PBRPoint();
    //vec3 PBR = PBRSpot();
    //vec3 PBR = PBRSpot() + PBRPoint() + PBRDirectional();
    
    PBR += light.ambient*texture(material.diffuse, fragment_texture_coordinate).rgb;

    frag_color = vec4(PBR, 1.0);

} 
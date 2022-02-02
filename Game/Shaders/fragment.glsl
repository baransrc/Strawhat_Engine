#version 330 core

#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679

out vec4 frag_color;

struct LightD {
    vec3  position;
    vec3  direction;
    float radius;
    float intensity;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	float shininess;

};

struct LightP {
    vec3  position;
    vec3  direction;
    float radius;
    float intensity;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
}; 

struct LightS {
    vec3  position;
    vec3  direction;
    float radius;
    float intensity;
    float inner;
    float outer;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
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

uniform vec3 camera_position;

uniform sampler2D input_texture;

uniform LightD lightD;
uniform LightP lightP;
uniform LightS lightS;
uniform Material material;

in vec3 world_normal;

vec3 SchlickFresnel(const vec3 f0, float cos_theta)
{
    return f0 + (vec3(1.0) - f0) * pow(1.0 - cos_theta, 5.0);
}

vec3 PBRDirectional()
{
    // Diffuse
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(-lightD.direction);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    // Specular
    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((lightD.shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, lightD.shininess);
    vec3 PRBDirectional = lightD.diffuse * (step1 + step2) * NdotL * lightD.intensity;

    return PRBDirectional;
}

vec3 PBRPoint()
{
    // Diffuse
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(lightP.position - fragment_position);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    // Specular
    vec3 view_direction = normalize(camera_position - fragment_position);
    float VdotR =  max(dot(view_direction, reflect(-L, fragment_normal)), 0.0001);

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);

    // attenuation
    float light_distance    = length(lightP.position - fragment_position);
    float attenuation = max(pow(max(1 - pow(light_distance/lightP.radius, 4), 0.0), 2.0), 0.0) / ((light_distance * light_distance) + 1);

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((lightP.shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, lightP.shininess);
    vec3 PBRPoint = lightP.diffuse * (step1 + step2) * NdotL * attenuation * lightP.intensity;

    return PBRPoint;
}

vec3 PBRSpot()
{
    // Diffuse
    vec3 N  = normalize(fragment_normal);
    vec3 L = normalize(lightS.position - fragment_position);

    float NdotL = max(dot(N,L), 0.0001);

    vec3 diffuse_color = texture(material.diffuse, fragment_texture_coordinate).rgb;

    // Specular
    vec3 view_direction = normalize(camera_position - fragment_position);
    vec3 R = reflect(-L, fragment_normal);
    float VdotR =  max(dot(view_direction, R), 0.0001);

    vec3 fresnel = SchlickFresnel(texture(material.specular, fragment_texture_coordinate).rgb, NdotL);
    
    // Cone attenuation
    float cone_attenuation = 0.0;
    vec3 dir = normalize(lightS.direction);
    float C = dot(L, dir);
    float cos_inner = cos(lightS.inner);
    float cos_outer = cos(lightS.outer);
    if (C > cos_inner) {
        cone_attenuation = 1.0;
    }else if (C > cos_outer){
        cone_attenuation = (C - cos_outer)/(cos_inner - cos_outer);
    }

    // Attenuation
    float light_distance = dot(L, dir);
    float attenuation = pow(max(1 - pow(light_distance/lightS.radius, 4), 0.0), 2.0) / ((light_distance * light_distance) + 1);

    vec3 step1 = (diffuse_color * (vec3(1.0) - texture(material.specular, fragment_texture_coordinate).rgb)) / PI;
    vec3 step2 = ((lightS.shininess + 2.0) / (2.0 * PI)) * fresnel * pow(VdotR, lightS.shininess);
    // Adjusting the cosinus with the 1 - on the attenuation formula part 
    vec3 PBRSpot = lightS.diffuse * (step1 + step2) * NdotL * ((1-attenuation) * (1-cone_attenuation)) * lightS.intensity;

    return PBRSpot;
 
}

void main()
{

    //vec3 PBR = PBRDirectional();
    //vec3 PBR = PBRPoint();
    //vec3 PBR = PBRSpot();
    vec3 PBR = PBRSpot() + PBRPoint() + PBRDirectional();

    // Ambient Light
    PBR += vec3(0.2, 0.2, 0.2)*texture(material.diffuse, fragment_texture_coordinate).rgb;

    frag_color = vec4(PBR, 1.0);

} 
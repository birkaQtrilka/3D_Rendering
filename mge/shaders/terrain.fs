#version 400

struct DirectionalLight
{
    float intensity;
    vec3 color;
    vec3 direction;
};

uniform DirectionalLight directionalLight;

uniform sampler2D splatMap;

uniform sampler2D diffuseTexture0;
uniform sampler2D diffuseTexture1;
uniform sampler2D diffuseTexture2;
uniform sampler2D diffuseTexture3;
uniform sampler2D diffuseTexture4;

//uniform vec3 diffuseColor;
uniform vec3 ambientClr;
uniform float ambientIntensity;
uniform vec4 viewerPosV;
uniform float specularIntensity;
uniform float specularPower;

in vec2 texCoord;
in vec4 vertexWorldPos;
in vec4 fNormal;

out vec4 fragment_color;

vec4 CalcDirLight(DirectionalLight light, vec3 norm, vec3 fragPos)
{
    vec3 ambient = ambientClr * ambientIntensity;

    vec3 diffuseMath = max( dot(norm, light.direction ) , 0) * light.intensity * light.color;

    vec3 viewDir = normalize(vec3(viewerPosV) - fragPos);
    vec3 reflection = reflect(-light.direction, norm);
    vec3 specular = pow(max( dot(viewDir, reflection),0.0), specularPower) * light.color  * specularIntensity;
    //return vec4(norm,1);
    return vec4(diffuseMath + ambient + specular, 1);
}

void main( void ) {
    vec4 t = vec4(0,0,0,1); //+ vec4(diffuseColor,0)
    vec4 splatValue = texture(splatMap, texCoord);
    //t += texture(diffuseTexture0,texCoord) * (1-splatValue.a);
    //t = vec4(splatValue.a);
    //g is sand
    //r is grass
    //b is rock
    //a is snow
    t += texture(diffuseTexture0,texCoord) * splatValue.r;
    t += texture(diffuseTexture1,texCoord) * splatValue.g;
    t += texture(diffuseTexture2,texCoord) * splatValue.b;
    t += texture(diffuseTexture3,texCoord) * splatValue.a;

	vec3 norm = normalize(vec3(fNormal));
    vec3 fragPos = vec3(vertexWorldPos);
    vec4 totalLights = CalcDirLight(directionalLight, norm, fragPos);

	fragment_color = t * totalLights;
}
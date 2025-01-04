#version 400


uniform sampler2D diffuseTexture;

uniform vec3 diffuseColor;
uniform vec3 ambientClr;
uniform float ambientIntensity;
uniform vec4 viewerPosV;
uniform float specularIntensity;
uniform float specularPower;

uniform int lightCount;

struct PointLight
{
  vec4 posAndIntensity;
  vec4 clrAndAttenuation;
};

struct DirectionalLight
{
    float intensity;
    vec3 color;
    vec3 direction;
};

layout(std140, binding=1) uniform LightingBlock {
  PointLight lights [100];
};

uniform DirectionalLight directionalLight;

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

    return vec4(diffuseMath + ambient + specular, 1);
}

vec4 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos)
{
    vec3 lightColor = vec3(light.clrAndAttenuation);
    float attenuation = light.clrAndAttenuation.a;
    vec3 lightPos = vec3(light.posAndIntensity);
    float lightIntensity = light.posAndIntensity.a;

    //ambient
    vec3 ambient = ambientClr * ambientIntensity;

    //diffuse
  	vec3 lightDir = normalize(lightPos - fragPos);
 	vec3 diffuseMath = max( dot(norm, lightDir ) , 0) * lightIntensity * lightColor;
     //attenuation
    float distance = max(distance(lightPos, fragPos),0.001f);
    diffuseMath /=  attenuation * distance;

    //specular
    vec3 viewDir = normalize(vec3(viewerPosV) - fragPos);
    vec3 reflection = reflect(-lightDir, norm);
    vec3 specular = pow(max( dot(viewDir, reflection),0.0f), specularPower) * lightColor  * specularIntensity;

    return vec4(diffuseMath + ambient + specular, 1);
}

void main( void ) {
	vec4 texture = texture(diffuseTexture,texCoord) + vec4(diffuseColor,0);
	vec3 norm = normalize(vec3(fNormal));
    vec3 fragPos = vec3(vertexWorldPos);
    vec4 totalLights = CalcDirLight(directionalLight, norm, fragPos);

    for(int i = 0; i < lightCount; i++)
    {
        totalLights += CalcPointLight(lights[i],norm,fragPos);
    }

	fragment_color = texture * totalLights;
}


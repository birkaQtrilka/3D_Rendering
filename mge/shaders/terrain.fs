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

uniform mat4 mMatrix;
uniform float maxHeight;

uniform sampler2D heightMap;

in vec2 texCoord;
in vec4 vertexWorldPos;
in vec4 fLocalPos;

out vec4 fragment_color;

vec4 Triplanar(sampler2D tex, vec4 coord, vec3 norm)
{
    vec4 result = vec4(0);

    result += texture(tex, coord.yz)* abs(dot(vec3(1.0f,0.0f,0.0f), norm));
    result += texture(tex, coord.zx)* abs(dot(vec3(0.0f,1.0f,0.0f), norm));
    result += texture(tex, coord.xy)* abs(dot(vec3(0.0f,0.0f,1.0f), norm));
    return result;
}

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
    ivec2 texSize = textureSize(heightMap, 0);
    vec2 texelSize = 1.0f / vec2(texSize);
    float heightUp = texture(heightMap,texCoord + vec2(0,texelSize.y)).x * maxHeight;
    float heightDown = texture(heightMap,texCoord - vec2(0,texelSize.y)).x * maxHeight;
    float heightRight = texture(heightMap,texCoord + vec2(texelSize.x,0)).x * maxHeight;
    float heightLeft = texture(heightMap,texCoord - vec2(texelSize.x,0)).x * maxHeight;

    //calulating normal
    float dist = .1f;
    vec3 axisA = vec3(0.0f, heightUp,dist) - vec3(0, heightDown,-dist) ;
    vec3 axisB = vec3(dist, heightRight,0.0f) - vec3(-dist, heightLeft,0.0f);


	vec3 norm =  normalize(vec3( mMatrix * vec4(cross(axisA,axisB),0)) );
    vec3 fragPos = vec3(vertexWorldPos);
    vec4 totalLights = CalcDirLight(directionalLight, norm, fragPos);

    vec4 splatValue = texture(splatMap, texCoord);
    //t = vec4(splatValue.a);
    //g is sand
    //r is grass
    //b is rock
    //a is snow
    vec4 t = vec4(0,0,0,1);
    t += Triplanar(diffuseTexture0, fLocalPos, norm) * splatValue.r;
    t += Triplanar(diffuseTexture1, fLocalPos, norm) * splatValue.g;
    t += Triplanar(diffuseTexture2, fLocalPos, norm) * splatValue.b;
    t += Triplanar(diffuseTexture3, fLocalPos, norm) * (1 - splatValue.a);


	fragment_color = t * totalLights;
}
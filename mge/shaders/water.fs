#version 400
#define PI 3.14159f;

struct DirectionalLight
{
    float intensity;
    vec3 color;
    vec3 direction;
};

uniform DirectionalLight directionalLight;

uniform vec3 diffuseColor;
uniform vec3 ambientClr;
uniform float ambientIntensity;
uniform vec4 viewerPosV;
uniform float specularIntensity;
uniform float specularPower;

uniform float waveAmplitude;
uniform float waveLength;
uniform float waveSpeed;
uniform float time;

uniform mat4 mMatrix;

in vec2 texCoord;
in vec4 vertexWorldPos;
in vec3 vertexLocalPos;
in vec4 fNormal;
out vec4 fragment_color;

vec4 CalcDirLight(DirectionalLight light, vec3 norm, vec3 fragPos)
{
    vec3 ambient = ambientClr * ambientIntensity;

    vec3 diffuseMath = max( dot(norm, light.direction ) , 0.0f) * light.intensity * light.color;

    vec3 viewDir = normalize(vec3(viewerPosV) - fragPos);
    vec3 reflection = reflect(-light.direction, norm);
    vec3 specular = pow(max( dot(viewDir, reflection),0.0f), specularPower) * light.color  * specularIntensity;
    return vec4(diffuseMath + ambient + specular, 1.0f);
}

vec3 GetPos(vec3 pos)
{
    float speed = waveSpeed;
    float hz = 2.0f * 3.14159f / waveLength;
    float f = hz * (pos.x - speed * time);
    float a = waveAmplitude / hz;
    pos.x += a * cos(f);
    pos.y = a * sin(f);

    vec3 tangent = normalize(vec3(1.0f - hz * waveAmplitude * sin(f), hz * waveAmplitude * cos(f), 0.0f));
    vec3 normal = vec3(-tangent.y, tangent.x, 0.0f);
    //fNormal = mMatrix * vec4(normal,0.0f);

    return pos;
}

void main( void ) {

    //vec4 v = GetPos(vertexLocalPos);
     vec3 fragPos = vec3(vertexWorldPos);

     //calulating normal
     float dist = .001f;
     vec3 heightUp = GetPos(fragPos + vec3(0,0,dist) );
     vec3 heightDown = GetPos(fragPos - vec3(0,0,dist) );
     vec3 heightRight = GetPos(fragPos + vec3(dist,0,0) );
     vec3 heightLeft = GetPos(fragPos - vec3(dist,0,0) );

     vec3 axisA = normalize(heightUp - heightDown);//this is nothing, should be 0
     vec3 axisB = normalize(heightRight - heightLeft);
     //fragment_color = vec4(cross(axisA,axisB),1.0f);

     vec3 norm = normalize( vec3( mMatrix * vec4(cross(axisA,axisB),1.0f) ));
     vec4 totalLights = CalcDirLight(directionalLight, norm, fragPos);

	fragment_color = vec4(diffuseColor,1) * totalLights;
}
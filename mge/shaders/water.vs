#version 400
#define PI 3.14159f;


layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform	mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform float waveAmplitude;
uniform float waveLength;
uniform float waveSpeed;
uniform float time;

out vec4 fNormal;

out vec2 texCoord;
out vec4 vertexWorldPos;
out vec3 vertexLocalPos;

vec4 GetPos(vec4 pos)
{
    float speed = waveSpeed;
    float hz = 2.0f * 3.14159f / waveLength;
    float f = hz * (pos.x - speed * time);
    float a = waveAmplitude / hz;
    pos.x += a * cos(f);
    pos.y = a * sin(f);

    vec3 tangent = normalize(vec3(1.0f - hz * waveAmplitude * sin(f), hz * waveAmplitude * cos(f), 0.0f));
    vec3 normal = vec3(-tangent.y, tangent.x, 0.0f);
    fNormal = mMatrix * vec4(normal,0.0f);

    return pos;
}

void main( void ){
    vertexWorldPos = GetPos(mMatrix *  vec4(vertex, 1.0f) );
	texCoord = uv;
	vertexLocalPos = vec3( inverse(mMatrix) * vertexWorldPos );
	gl_Position = mvpMatrix *  vec4(vertexLocalPos,1.0f);
}
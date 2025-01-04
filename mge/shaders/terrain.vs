#version 400
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform	mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform float maxHeight;

uniform sampler2D heightMap;

//out vec4 fNormal;

out vec2 texCoord;
out vec4 vertexWorldPos;

void main( void ){
    float heightCurrent = texture(heightMap,uv).x * maxHeight;

    vec4 vertexLocalPos = vec4(vertex + vec3(0.0f,heightCurrent,0.0f), 1.f);

	texCoord = uv;
	vertexWorldPos = mMatrix * vertexLocalPos;
    //fNormal = mMatrix * vec4(normalize(cross(axisA,axisB)),0.0f);
	gl_Position = mvpMatrix * vertexLocalPos;
}
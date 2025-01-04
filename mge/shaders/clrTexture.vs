#version 400
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform	mat4 mvpMatrix;
uniform mat4 mMatrix;

out vec2 texCoord;
out vec4 vertexWorldPos;
out vec4 fNormal;

void main( void ){
	gl_Position = mvpMatrix * vec4(vertex, 1.0f);
	texCoord = uv;
	vertexWorldPos = mMatrix * vec4(vertex, 1.0f);
	fNormal = mMatrix * vec4(normal, 0.0f);
}
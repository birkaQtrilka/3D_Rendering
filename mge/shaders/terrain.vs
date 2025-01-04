#version 400
layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

uniform	mat4 mvpMatrix;
uniform mat4 mMatrix;
uniform float maxHeight;

uniform sampler2D heightMap;

out vec4 fNormal;

out vec2 texCoord;
out vec4 vertexWorldPos;

void main( void ){
    ivec2 texSize = textureSize(heightMap, 0);
    vec2 texelSize = 1.0f / vec2(texSize);
    float heightCurrent = texture(heightMap,uv).x * maxHeight;
    float heightUp = texture(heightMap,uv + vec2(0,texelSize.y)).x * maxHeight;
    float heightDown = texture(heightMap,uv - vec2(0,texelSize.y)).x * maxHeight;
    float heightRight = texture(heightMap,uv + vec2(texelSize.x,0)).x * maxHeight;
    float heightLeft = texture(heightMap,uv - vec2(texelSize.x,0)).x * maxHeight;

    //calulating normal
    float dist = .005f;
    vec3 axisA = vec3(0.0f, heightUp,dist) - vec3(0, heightDown,-dist) ;
    vec3 axisB = vec3(dist, heightRight,0.0f) - vec3(-dist, heightLeft,0.0f);

    vec4 vertexLocalPos = vec4(vertex + vec3(0.0f,heightCurrent,0.0f), 1.f);

	texCoord = uv;
	vertexWorldPos = mMatrix * vec4(vertex, 1.0f);
    fNormal = mMatrix * vec4(normalize(cross(axisA,axisB)),0.0f);
	gl_Position = mvpMatrix * vertexLocalPos;
}
#version 430 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in int inMeta;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out Vertex
{
	vec3 pos;
	int meta;
} outVert;

void main()
{
	vec4 wsPos = modelMatrix * vec4(inPos, 1.f);
	outVert.pos = vec3(wsPos);
	outVert.meta = inMeta;

	gl_Position = viewMatrix * wsPos;
}
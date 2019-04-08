#version 450 core

layout(location = 0) in vec4 inPos;
layout(location = 1) in vec3 inNorm;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out Vertex
{
	vec3 pos;
	vec3 norm;
} outVert;

void main()
{
	vec4 wsPos = modelMatrix * inPos;
	outVert.pos = vec3(wsPos);
	outVert.norm = inNorm;

	gl_Position = viewMatrix * wsPos;
}
#version 430 core

layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNorm;
layout(location = 2) in vec3 inTang;
layout(location = 3) in vec3 inBinorm;
layout(location = 4) in vec4 inColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out Vertex
{
	vec3 pos;
	vec3 norm;
	vec4 color;
} outVert;

void main()
{
	vec4 wsPos = modelMatrix * vec4(inPos, 1.f);
	outVert.pos = vec3(wsPos);
	outVert.norm = inNorm;
	outVert.color = vec4(mix(normalize(inPos + 1.f), vec3(1.f), 0.5f), 1.f);

	gl_Position = viewMatrix * wsPos;
}
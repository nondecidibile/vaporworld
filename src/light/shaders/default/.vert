#version 430 core

layout(location = 0) in vec4 inPos;

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
	outVert.norm = vec3(0.f, 1.f, 0.f);

	gl_Position = viewMatrix * wsPos;
}
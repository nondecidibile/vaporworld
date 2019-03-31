#version 430 core

layout(location = 0) out vec4 fColor;

in Vertex
{
	vec3 pos;
	vec3 norm;
	vec4 color;
} inVert;

void main()
{
	fColor = vec4(1.f);
}
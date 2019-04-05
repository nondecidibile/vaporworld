#version 430 core

layout(location = 0) out vec4 outColor;

in Vertex
{
	vec3 pos;
	vec3 norm;
} inVert;

void main()
{
	outColor = vec4(inVert.pos * inVert.pos.y, 1.f);
}
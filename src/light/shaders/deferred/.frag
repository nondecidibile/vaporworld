#version 450 core

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNorm;
layout(location = 2) out vec3 outColor;

in Vertex
{
	vec3 pos;
	vec3 norm;
} inVert;

void main()
{
	// Output fragment data
	outPos = inVert.pos;
	outNorm = inVert.norm;
	outColor = sin(inVert.pos) * 0.5f + 0.5f;
}
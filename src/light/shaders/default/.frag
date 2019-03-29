#version 430 core

uniform vec3 cameraLocation;
uniform vec3 lightPoints[128];
uniform int numLights;

layout(location = 0) out vec3 gPos;
layout(location = 1) out vec3 gNorm;
layout(location = 2) out vec4 gColor;

in Vertex
{
	vec3 pos;
	vec3 norm;
	vec3 tang;
	vec3 binorm;
	vec4 color;
} inVert;

void main()
{
	gPos = inVert.pos;
	gNorm = inVert.norm;
	gColor = inVert.color;
}
#version 430 core

// Geomtry shader specs
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

in Vertex
{
	vec3 pos;
	vec3 norm;
	vec3 tang;
	vec3 binorm;
	vec4 color;
} inVerts[];

out Vertex
{
	vec3 pos;
	vec3 norm;
	vec3 tang;
	vec3 binorm;
	vec4 color;
} outVert;

void main()
{
	vec3 d0 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 d1 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 norm = normalize(cross(d0, d1));

	for (uint i = 0; i < 3; ++i)
	{
		gl_Position = viewMatrix * gl_in[i].gl_Position;
		outVert.pos = inVerts[i].pos;
		outVert.norm = norm;
		outVert.color = inVerts[i].color;
		EmitVertex();
	}

	EndPrimitive();
}
#version 430 core

layout(location=0) in vec4 pos;
layout(location=1) in vec3 normal;
layout(location=3) in vec2 uv;

out VertexData{
    vec3 position;
    vec3 normal;
	vec2 uv;
} outVert;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

void main(){
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));

	vec4 ws_pos = modelMatrix*pos;
	outVert.position = ws_pos.xyz;
	outVert.normal = normalize(normalMatrix*normal);
	outVert.uv = uv;

    gl_Position = viewMatrix*ws_pos;
}
#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 viewMatrix;

out VertexData{
    vec3 position;
    vec3 normal;
} outVert;

void main(){

    vec3 l0 = vec3(gl_in[1].gl_Position - gl_in[0].gl_Position);
    vec3 l1 = vec3(gl_in[2].gl_Position - gl_in[0].gl_Position);
    vec3 normal = cross(normalize(l1),normalize(l0));

    for(int i=0; i<3; i++){
        outVert.position = gl_in[i].gl_Position.xyz;
        outVert.normal = normalize(normal);
        gl_Position = viewMatrix * gl_in[i].gl_Position;
        EmitVertex();
    }

    EndPrimitive();

}
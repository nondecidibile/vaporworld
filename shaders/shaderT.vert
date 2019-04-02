#version 430 core

layout(location=0) in vec4 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 color;

uniform mat4 modelMatrix;

void main(){

    gl_Position = modelMatrix*pos;
}
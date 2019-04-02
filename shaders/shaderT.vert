#version 430 core

layout(location=0) in vec4 pos;

uniform mat4 modelMatrix;

void main(){

    gl_Position = modelMatrix*pos;
}
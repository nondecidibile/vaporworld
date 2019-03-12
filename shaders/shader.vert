#version 430 core

layout(location=0) in vec4 pos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 fragmentPos;

void main(){

    fragmentPos = modelMatrix * pos;
    gl_Position = viewMatrix * modelMatrix * pos;
}
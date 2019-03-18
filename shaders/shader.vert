#version 430 core

layout(location=0) in vec4 pos;
layout(location=1) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 vColor;

void main(){

    float c = pos[1]/5;
    vColor = vec4(1-c,0,c,1);//color;
    gl_Position = viewMatrix * modelMatrix * pos;
}
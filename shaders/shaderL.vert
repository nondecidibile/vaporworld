#version 430 core

layout(location=0) in vec4 pos;
layout(location=1) in vec4 normal;
layout(location=2) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec4 vColor;

void main(){

    float c = pos[1]/5;
    vColor = vec4(0.2+c*2,0,0.5+c,1);
    gl_Position = viewMatrix * modelMatrix * pos;
}
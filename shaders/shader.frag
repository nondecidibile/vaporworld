#version 430 core

out vec4 color;
uniform vec4 modelColor;

in vec4 vColor;

void main(){

    color = vColor;
}
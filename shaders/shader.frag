#version 430 core

out vec4 color;
uniform vec4 modelColor;

in vec4 fragmentPos;

void main(){

    //float ambientStrength = 0.1;
    //vec3 ambient = ambientStrength * vec3(1.0,1.0,1.0);
    //color = vec4(ambient,1) * modelColor;

    color = modelColor;

}
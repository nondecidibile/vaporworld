#version 430 core

layout(location=0) in vec4 pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec4 color;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

out vec3 Normal;
out vec3 FragPos;

uniform vec3 cameraLocation;
uniform vec4 timeColor;
const float PI = 3.1415926535897932384626433832795;

void main(){

    Normal = normal;
    FragPos = vec3(modelMatrix * pos);

    float z = cameraLocation[2];
    float f = 0.05*sin(z/20)+0.025*cos(2*z/20)+0.03*sin(3*z/20);
    float d = FragPos[2] - cameraLocation[2];
    FragPos.y += f*pow(d,2);

    gl_Position = viewMatrix * vec4(FragPos, 1.f);
}
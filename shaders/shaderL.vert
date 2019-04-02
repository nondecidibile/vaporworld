#version 430 core

layout(location=0) in vec4 pos;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;

uniform vec3 cameraLocation;
uniform vec4 timeColor;
const float PI = 3.1415926535897932384626433832795;

out vec4 vColor;

void main(){

    vec3 FragPos = vec3(modelMatrix * pos);
    
    float x = timeColor.x;
    float r = 0.3;//0.5+0.5*sin(10*x);
    float g = 0.7;
    float b = 1.0;//0.5-0.5*sin(10*x);
    vColor = vec4(r,g,b,1);

    gl_Position = viewMatrix * vec4(FragPos, 1.f);
}
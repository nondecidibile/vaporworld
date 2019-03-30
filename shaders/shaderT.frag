#version 430 core

in vec3 Normal;
in vec3 FragPos;

out vec4 color;

void main(){

    vec3 lightPos = vec3(0,10,0);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * vec3(1,1,1); // light color
    
    vec3 result = diffuse * vec3(1,1,1); // object color
    
    color = vec4(0,0,0,1.0);
}
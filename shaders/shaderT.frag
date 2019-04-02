#version 430 core

in VertexData{
    vec3 position;
    vec3 normal;
} inVert;

uniform vec3 cameraLocation;

out vec4 color;

void main(){

    vec3 objectColor = vec3(1,1,1);
    float lightStrength = 50;
    vec3 lightPos[3];
    lightPos[0] = cameraLocation + vec3(0,10,25);
    lightPos[1] = cameraLocation + vec3(0,15,5);
    lightPos[2] = cameraLocation + vec3(0,15,-10);
    vec3 lightColor[3];
    lightColor[0] = vec3(0.75,0.0,0.75);
    lightColor[1] = vec3(0.05,0.0,0.3);
    lightColor[2] = vec3(0.2,0,0.4);

    vec3 ambient = vec3(0,0,0);
    vec3 result = ambient * objectColor;

    for(int i=0; i<3; i++){

        vec3 lightRay = inVert.position - lightPos[i];
        vec3 lightDir = normalize(lightRay);
        float lightDist = length(lightRay);

        vec3 reflectionDir = reflect(lightDir,inVert.normal);
        vec3 pixelDir = normalize(inVert.position-cameraLocation);

        float diff = max(dot(-inVert.normal, lightDir), 0.0) * exp(-lightDist/lightStrength);
        vec3 diffuse = diff * lightColor[i];

        vec3 specular = pow(max(dot(-reflectionDir,pixelDir),0),16) * exp(-lightDist/lightStrength) * lightColor[i];
        
        result += (diffuse + specular) * objectColor;
    }
    
    color = vec4(result,1.0);
}
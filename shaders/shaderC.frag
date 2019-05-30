#version 430 core

in VertexData{
    vec3 position;
    vec3 normal;
	vec2 uv;
} inVert;

layout(binding=0) uniform sampler2D diffuseTexture;
layout(binding=1) uniform sampler2D glossTexture;
layout(binding=2) uniform sampler2D specularTexture;

uniform vec3 cameraLocation;

out vec4 color;

void main(){

    vec3 objectColor = texture(diffuseTexture,inVert.uv).rgb;
	float objectGloss = texture(glossTexture,inVert.uv).r;
	float objectSpecular = texture(specularTexture,inVert.uv).r;

    vec3 lightPos[2];
    lightPos[0] = cameraLocation + vec3(-20,10,0);
    lightPos[1] = cameraLocation + vec3(0,10,0);
    vec3 lightColor[2];
    lightColor[0] = vec3(1.0,0.5,1.0);
    lightColor[1] = vec3(0.9,1.0,0.1);

    vec3 ambient = vec3(0,0,0);
    vec3 result = ambient * objectColor;

    for(int i=0; i<2; i++){

        vec3 lightRay = inVert.position - lightPos[i];
        vec3 lightDir = normalize(lightRay);
        float lightDist = length(lightRay);

        vec3 reflectionDir = reflect(lightDir,-inVert.normal);
        vec3 pixelDir = normalize(inVert.position-cameraLocation);

        float diff = max(dot(-inVert.normal, lightDir), 0.0) * exp(-lightDist/20);
        vec3 diffuse = diff * lightColor[i];

        vec3 specular = pow(max(dot(-reflectionDir,pixelDir),0),16*objectGloss) * objectSpecular * exp(-lightDist/20) * lightColor[i];
        
        result += (diffuse + specular) * objectColor;
    }
    
    color = vec4(result,1.0);
}
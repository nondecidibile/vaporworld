#version 450 core

layout(location = 0) out vec4 outColor;

/// Camera location
uniform vec3 cameraLocation;

/// Terrain texture
layout(binding = 0) uniform sampler2D texDiffuseX;
layout(binding = 1) uniform sampler2D texDiffuseY;
layout(binding = 2) uniform sampler2D texDiffuseZ;

in Vertex
{
	vec3 pos;
	vec3 norm;
} inVert;

void main()
{
	const vec3 uvw = inVert.pos * 2.f;

	const vec3 lightOrientation = normalize(vec3(0.5f, -1.f, -0.9f));
	const vec3 lightPos = vec3(0.f, 5.f, 0.f);
	const vec3 lightColor = normalize(vec3(1.f, 0.9f, 0.85f));
	const float lightRadius = 20.f;
	const float lightIntensity = 1.2f;
	const float specularStrength = 0.85f;
	const float specularFactor = 16.f;

	const vec3 lightRay = inVert.pos - lightPos;
	const vec3 lightDir = lightOrientation;//normalize(lightRay);
	const float ligthDist = length(lightRay);
	const vec3 reflectionDir = reflect(-lightDir, inVert.norm);
	const vec3 viewDir = normalize(inVert.pos - cameraLocation);

	const vec3 ambient = lightColor * 0.1f;
	const vec3 albedoX = /* texture(texDiffuseX, uvw.yz).rgb */ vec3(0.8f, 0.8f, 0.8f);
	const vec3 albedoY = /* texture(texDiffuseY, uvw.xz).rgb */ vec3(0.8f, 0.8f, 0.8f);
	const vec3 albedoZ = /* texture(texDiffuseZ, uvw.xy).rgb */ vec3(0.8f, 0.8f, 0.8f);
	//const vec3 albedo = mix(sin(inVert.pos) * 0.5f + 0.5f, abs(inVert.norm.rgr), 0.05f);
	const vec3 trilinearWeight = normalize(pow(abs(inVert.norm), vec3(3.4f)));
	const vec3 albedo = (albedoX * trilinearWeight.x + albedoY * trilinearWeight.y + albedoZ * trilinearWeight.z) * (sin(inVert.pos) * 0.5f + 0.5f);
	//const vec3 albedo = sin(inVert.pos) * 0.5f + 0.5f;
	const float diffuse = max(dot(lightDir, -inVert.norm), 0.f) * lightIntensity;// * exp(-ligthDist / lightRadius);
	const float specular = pow(max(dot(reflectionDir, viewDir), 0.f), 16.f) * specularStrength * exp(-ligthDist / lightRadius);

	const vec3 finalColor = (ambient + diffuse + specular) * lightColor * albedo;

	outColor = vec4(mix(albedo, finalColor, 0.9f), 1.f);
}
#version 450 core

layout(location = 0) out vec4 outColor;

/// Camera location
uniform vec3 cameraLocation;

/// Terrain texture
layout(binding = 0) uniform sampler2D tex;

in Vertex
{
	vec3 pos;
	vec3 norm;
} inVert;

void main()
{
	const vec3 lightOrientation = normalize(vec3(0.5f, -1.f, 2.f));
	const vec3 lightPos = vec3(0.f, 5.f, 0.f);
	const vec3 lightColor = normalize(vec3(1.f, 0.8f, 0.8f));
	const float lightRadius = 20.f;
	const float lightIntensity = 1.2f;
	const float specularStrength = 0.15f;
	const float specularFactor = 16.f;

	const vec3 lightRay = inVert.pos - lightPos;
	const vec3 lightDir = lightOrientation;//normalize(lightRay);
	const float ligthDist = length(lightRay);
	const vec3 reflectionDir = reflect(-lightDir, inVert.norm);
	const vec3 viewDir = normalize(inVert.pos - cameraLocation);

	const vec3 ambient = normalize(vec3(1.f) - lightColor) * 1.2f;
	/* const vec3 albedoX = texture(tex, inVert.pos.yz).rgb;
	const vec3 albedoY = texture(tex, inVert.pos.xz).rgb;
	const vec3 albedoZ = texture(tex, inVert.pos.xy).rgb; */
	const vec3 albedo = sin(inVert.pos) * 0.5f + 0.5f;
	const float diffuse = max(dot(lightDir, -inVert.norm), 0.f) * lightIntensity;// * exp(-ligthDist / lightRadius);
	const float specular = pow(max(dot(reflectionDir, viewDir), 0.f), 16.f) * specularStrength * exp(-ligthDist / lightRadius);

	const vec3 finalColor = (ambient + diffuse + specular) * lightColor * albedo;

	outColor = vec4(mix(albedo * max(inVert.pos.y, 0.f), finalColor, 0.5f), 1.f);
}
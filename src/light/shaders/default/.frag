#version 430 core

uniform vec3 cameraLocation;
uniform vec3 lightPoints[128];
uniform int numLights;

out vec4 fColor;

in Vertex
{
	vec3 pos;
	vec3 norm;
	vec3 tang;
	vec3 binorm;
	vec4 color;
} inVert;

void main()
{
	const float ambient = 0.1f;
	const float specular = 0.6f;

	vec3 result = vec3(0);
	
	for (uint i = 0; i < numLights; ++i)
	{
		const vec3 lightPos = lightPoints[i];
		const vec3 lightColor = vec3(0.f, 0.1f, 1.f);
		const float lightIntensity = 2.5f;
		const float lightMaxDist = 20.f;
		const vec3 lightRay = inVert.pos - lightPos;
		const vec3 lightDir = normalize(lightRay);
		const float lightMultiplier = lightIntensity * exp(-length(lightRay) / lightMaxDist);
		const vec3 diffuse = max(dot(inVert.norm, lightDir), 0.f) * lightMultiplier * lightColor;

		const vec3 viewDir = normalize(inVert.pos - cameraLocation);
		const vec3 reflectionDir = reflect(-lightDir, inVert.norm);
		const float spec = pow(max(dot(viewDir, reflectionDir), 0.f), 100.f);

		result += (diffuse + specular * spec * lightColor) * inVert.color.rgb;
	}

	{
		const vec3 lightColor = vec3(1.f, 0.2f, 0.f);
		const float lightIntensity = 0.4f;
		const vec3 lightDir = vec3(1.f, -2.f, 0.f);
		const vec3 diffuse = max(dot(inVert.norm, lightDir), 0.f) * lightIntensity * lightColor;

		result += diffuse * inVert.color.rgb;
	}

	result += ambient * inVert.color.rgb;

	fColor = vec4(result, inVert.color.a);
}
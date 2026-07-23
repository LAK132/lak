#version 330 core

smooth in vec4 fColor;
smooth in vec3 fEye;
smooth in vec3 fNormal;
smooth in vec3 fPosition;
smooth in vec2 fTexCoord;

uniform sampler2D albedo;

uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform float shininess;

#define MAX_LIGHTS 6
struct light {
	vec3 position;
	vec4 color;
};
uniform int lightCount;
uniform light lights[MAX_LIGHTS];

out vec4 pColor;

void main()
{
	vec3 normal = normalize(fNormal);
	vec3 viewDir = normalize(fEye - fPosition);
	vec4 texColor = texture(albedo, fTexCoord); // * fColor;

	pColor = ambient * mix(fColor, texColor, texColor.w);

	for (int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
	{
		vec3 lightDir = normalize(lights[i].position - fPosition);
		float dNL = max(dot(normal, lightDir), 0.0f);
		vec4 color = diffuse;
		color = diffuse * texColor;
		vec4 lambert = color * lights[i].color * dNL;

		vec3 halfVec = normalize(lightDir + viewDir);
		float dNH = max(dot(normal, halfVec), 0.0f);
		vec4 phong = specular * lights[i].color * pow(dNH, shininess);
		pColor += lambert + phong;
	}
}

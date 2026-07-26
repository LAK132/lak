#version 330 core

smooth in vec4 fColor;
smooth in vec3 fEye;
smooth in vec3 fNormal;
smooth in vec3 fPosition;
smooth in vec2 fTexCoord;

uniform sampler2D albedo;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;

#define MAX_LIGHTS 32
struct light {
	vec3 position;
	vec3 color;
};
uniform int lightCount;
uniform light lights[MAX_LIGHTS];

out vec4 pColor;

float lambert_factor(vec3 normal, vec3 light_dir)
{
	return max(dot(normal, light_dir), 0.0);
}

float phong_factor(vec3 normal, vec3 light_dir, vec3 view_dir, float shininess)
{
	vec3 half_dir = normalize(light_dir + view_dir);
	return pow(lambert_factor(normal, half_dir), shininess);
}

void main()
{
	vec3 normal = normalize(fNormal);
	vec3 view_dir = normalize(fEye - fPosition);
	vec4 tex_colour = texture(albedo, fTexCoord); // * fColor;

	pColor = vec4(ambient, 1.0) * mix(fColor, tex_colour, tex_colour.w);

	for (int i = 0; i < lightCount && i < MAX_LIGHTS; i++)
	{
		vec3 frag_to_light = lights[i].position - fPosition;
		vec3 light_dir = normalize(frag_to_light);
		vec4 light_colour = vec4(lights[i].color / length(frag_to_light), 1.0);

		float lambert = lambert_factor(normal, light_dir);
		float phong = phong_factor(normal, light_dir, view_dir, shininess);

		pColor += light_colour * (
			(vec4(diffuse, 1.0) * tex_colour * lambert) +
			(vec4(specular, 1.0) * phong));
	}
}

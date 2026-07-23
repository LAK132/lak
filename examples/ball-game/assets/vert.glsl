#version 330 core

in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
in vec2 vTexCoord;

uniform mat4 projview;
uniform mat4 invprojview;
uniform mat4 model;

out vec4 fColor;
out vec3 fNormal;
out vec2 fTexCoord;
out vec3 fPosition;
out vec3 fEye;

const vec4 WUP = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
	vec4 vertpos = model * vPosition; // object -> world space

	fTexCoord = vTexCoord;
	fColor = vColor;
	fEye = vec3(WUP * invprojview);   // screen -> camera -> world space
	fNormal = mat3(model) * vNormal;  // object -> world space (no translation/scale)
	fPosition = vertpos.xyz;

	gl_Position = projview * vertpos; // world -> camera -> screen space
}

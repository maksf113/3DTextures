#version 430 core

layout(location = 0) in vec3 aPos;

uniform mat4 lightVP;
uniform mat4 model;

out vec3 vPos;

void main()
{
	gl_Position = lightVP * model * vec4(aPos, 1.0);
	vPos = aPos;
}
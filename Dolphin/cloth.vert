#version 430 core

layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTexUV;

uniform mat4 camMatrix;
uniform mat4 model;

out vec2 texCoord;

void main()
{
	gl_Position = camMatrix * model * vec4(aPosition, 1.0);
	texCoord = aTexUV;
}

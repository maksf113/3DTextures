#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 Normal;
out vec3 vPos;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat3 transposeInverse;

void main()
{
	crntPos = vec3(model * vec4(aPos, 1.0f));
    Normal = transposeInverse * aNormal;
    vPos = aPos;

    gl_Position = camMatrix * vec4(crntPos, 1.0); 
}
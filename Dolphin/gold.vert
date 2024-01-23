#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 3) in vec2 aTex;

out vec3 crntPos;
out vec3 vPos;
out vec3 Normal;
out vec3 reflectionDir;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat3 transposeInverse;
uniform vec3 camPos;

void main()
{
	crntPos = vec3(model * vec4(aPos, 1.0f));
    Normal = normalize(transposeInverse * aNormal);
    //Normal = aNormal;
    vec3 viewDir = normalize(crntPos - camPos);
    reflectionDir = reflect(viewDir, Normal);
    vPos = aPos;
    gl_Position = camMatrix * vec4(crntPos, 1.0); 
}
#version 430 core
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 vPos;
in vec3 reflectionDir;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler3D tex3D;
uniform samplerCube cubemap;

uniform vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 camPos;
uniform float shininess;
uniform vec3 baseColor = vec3(0.8549, 0.6471, 0.1255);
uniform vec3 veinColor = vec3(0.01, 0.01, 0.05);

vec4 directLight()
{
	vec3 lightDirection =  normalize(vec3(0.0f, -1.0f, 0.0f));

	vec3 normal = normalize(Normal);

	float diffuse = max(0.0f, dot(normal, -lightDirection));

	float ambient = 0.40f;

	float specularLight = 0.90f;
	vec3 viewDirection  = normalize(camPos - crntPos);
	vec3 reflection = reflect(lightDirection, normal);
	float specAmount = pow(max(0.0f, dot(viewDirection, reflection)), 8);
	float specular = specAmount * specularLight * shininess;

	return lightColor * (diffuse + specular + ambient);
}



void main()
{
	vec3 texCoords = vPos / 197.0;
	texCoords += vec3(0.3, 0.2, -0.0);
	vec4 texColor = texture(tex3D, texCoords);
	vec4 reflectedColor = texture(cubemap, reflectionDir);
	vec4 reflectedMix = mix(texColor, reflectedColor, 0.5);
	float factor = (texColor.r - veinColor.r) / (baseColor.r - veinColor.r);
    FragColor = mix(texColor, reflectedMix, factor); 
} 
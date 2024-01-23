#version 430 core
out vec4 FragColor;

in vec3 crntPos;
in vec3 Normal;
in vec3 vPos;

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform sampler3D tex3D;

uniform vec4 lightColor = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 lightPos;
uniform vec3 camPos;
uniform float shininess;



vec4 directLight()
{
	vec3 lightDirection =  normalize(vec3(0.0f, -1.0f, 0.0f));

	vec3 normal = normalize(Normal);

	float diffuse = max(0.0f, dot(normal, -lightDirection));

	float ambient = 0.40f;

	float specularLight = 0.90f;
	vec3 viewDirection  = normalize(crntPos - camPos);
	vec3 reflection = reflect(lightDirection, normal);
	float specAmount = pow(max(0.0f, dot(-viewDirection, reflection)), 8);
	float specular = specAmount * specularLight * shininess;

	return lightColor * (diffuse + specular + ambient);
}


void main()
{
	vec3 texCoords = vPos / 150.0;
	texCoords += vec3(0.3, 0.25, -0.15);
	vec4 texColor = texture(tex3D, texCoords);
    FragColor = directLight() * texColor; 
} 
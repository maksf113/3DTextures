#version 430 core

in vec3 vPos;

uniform float time;
uniform sampler3D dissTex;
uniform bool dissolve;

void main() 
{
	if(dissolve)
	{
		vec3 texCoords = vPos / 150.0;
		texCoords += vec3(0.3, 0.25, -0.15);
		vec4 dissColor = texture(dissTex, texCoords);
		float val = length(dissColor.rgb);
		float fluct = 0.05;
		float treshold = 0.35 + sin(time) * fluct;
		if(val < treshold)
			discard;
	}
}
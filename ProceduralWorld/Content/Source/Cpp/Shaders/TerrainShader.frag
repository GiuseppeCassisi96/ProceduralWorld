#version 460 core

in vec3 Normal;
out vec4 fragColor;

void main()
{
	vec3 normalColor = normalize(Normal);
	fragColor = vec4(Normal, 1.0);
}

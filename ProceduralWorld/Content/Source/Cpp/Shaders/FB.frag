#version 460 core

in vec2 vTexCoords;
out vec4 fragColor;

uniform sampler2D frameTexture;

void main()
{
	fragColor =  texture(frameTexture, vTexCoords);
}
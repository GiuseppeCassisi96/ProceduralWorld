#version 460 core


//Input vars
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aUVCoord;

uniform mat4 model;         
uniform mat4 view;            
uniform mat4 proj;    

out vec3 Normal;

void main()
{
	Normal = aNormal;
	gl_Position = proj * view * model * vec4(aPos,1.0);
}

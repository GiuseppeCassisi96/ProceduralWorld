#version 460 core


//Input vars
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUVCoord;

uniform mat4 model;         
uniform mat4 view;            
uniform mat4 proj; 
uniform mat3 normalMatrix;
uniform vec3 lightDir;
uniform vec3 cameraPos;

out vec3 vNormal;
out vec3 viewDir;
out vec3 vlightDir;
out vec2 vUVCoord;
void main()
{
	//Vertex position in world space
	vec4 worldVertexPos = model * vec4(aPos,1.0);
	//Camera position in world space
	vec4 worldcameraPos = model * vec4(cameraPos, 1.0);
	//The view direction will be in world space
	viewDir = normalize(worldcameraPos.xyz - worldVertexPos.xyz);
	//Light direction in world space
	vlightDir = (model * vec4(lightDir, 0.0)).xyz;
	vNormal = normalize( normalMatrix * aNormal);
	vUVCoord = aUVCoord;
	gl_Position = proj * view * worldVertexPos;
}

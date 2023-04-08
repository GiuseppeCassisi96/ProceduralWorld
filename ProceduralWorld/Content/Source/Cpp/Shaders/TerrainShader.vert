#version 460 core


//Input vars
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUVCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBiTangent;
layout (location = 5) in vec3 aColor;

subroutine vec4 VertexComp();

uniform mat4 model;         
uniform mat4 view;            
uniform mat4 proj; 
uniform mat3 normalMatrix;
uniform vec3 lightDir;
uniform vec3 cameraPos;
subroutine uniform VertexComp sVertex;
uniform mat4 skyview;            
uniform mat4 skyproj;

out vec3 vNormal;
out vec3 viewDir;
out vec3 vColor;
out vec3 vlightDir;
out vec2 vUVCoord;
out vec3 skyVUVCoord;
out float vHeight;

subroutine (VertexComp)
vec4 TreeAndTerrain()
{
	//Vertex position in world space
	vec4 worldVertexPos = model * vec4(aPos,1.0);
	vHeight = worldVertexPos.y;
	//Camera position in world space
	vec4 worldcameraPos = model * vec4(cameraPos, 1.0);
	//The view direction will be in world space
	viewDir = normalize(worldcameraPos.xyz - worldVertexPos.xyz);
	//Light direction in world space
	vlightDir = (model * vec4(lightDir, 0.0)).xyz;
	vNormal = normalize( normalMatrix * aNormal);
	vUVCoord = aUVCoord;
	vColor = aColor;
	return proj * view * worldVertexPos;
}

subroutine(VertexComp)
vec4 SkyBoxVert()
{
	skyVUVCoord = aPos;
	vec4 pos = proj * view * vec4(aPos, 1.0);
	return pos.xyww;
}
void main()
{
	gl_Position = sVertex();
}

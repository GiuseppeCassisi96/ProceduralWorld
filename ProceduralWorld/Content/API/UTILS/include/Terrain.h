#pragma once
#define GLEW_STATIC
#include <vector>
#include <GLM/include/fwd.hpp>
#include "Shader.h"
#include "Texture.h"
#include "UTILS/include/Movement.h"
#include "UTILS/include/Mesh.h"
class Terrain
{


public:
	struct TerrainVertex
	{
		glm::vec3 Position;
		glm::vec2 UVCoord;
	};
	Terrain(int numberOfPatch, HeightMap& ElevationMap);
	void DrawTerrain(glm::mat4& terrainModel, glm::mat4& cameraView, Shader& terrainShader);
	std::vector<TerrainVertex> vertices;
	std::vector<glm::vec3> NormalMap;
private:
	void SetupTerrain();
	void ComputeNormalMap();
	int numberOfPatch;
	HeightMap& ElevationMap;
	unsigned int VAO, VBO;
	std::vector<unsigned int> indices;
	
};
 


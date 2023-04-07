#pragma once
#include "vector"
#include "HeightMap.h"
#include "GLM/include/vec2.hpp"
#include "GLM/include/vec3.hpp"
#include "Constants.h"
#include "Include/glad.h"

struct TerrainVertex
{
	glm::vec3 Position;
	glm::vec2 UVCoords;
	glm::vec3 Normal;
};
class TerrainGeneration
{
public:
	TerrainGeneration(HeightMap& ElevationMap);
	void DrawTerrain();
	void ReComputeMesh();

	HeightMap ElevationMap;

	struct TerrainMat
	{
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		int Lawn = 3;
		int Forest = 4;
		int Mountain = 5;
		float Kd = 0.5f;
		float Ks = 0.4f;
		float Ka = 0.1f;
		float shininess = 5000.0f;
	};
	TerrainMat terrainMaterial;
	std::vector<TerrainVertex> vertices;
private:
	void ComputeMesh();
	void SetupBuffers();
	void ComputeNormals();
	std::vector<int> indices;
	unsigned int VBO, VAO, EBO;
};

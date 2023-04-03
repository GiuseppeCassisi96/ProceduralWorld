#pragma once
#include "vector"
#include "HeightMap.h"
#include "GLM/include/vec2.hpp"
#include "GLM/include/vec3.hpp"
#include "Constants.h"
#include "Include/glad.h"


class TerrainGeneration
{
public:
	TerrainGeneration(HeightMap& ElevationMap);
	void DrawTerrain();
	void ReComputeMesh();
	HeightMap ElevationMap;

	struct TerrainMat
	{
		glm::vec3 diffusiveColor = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 ambientColor = glm::vec3(1.0f, 0.0f, 0.0f);
		float Kd = 0.5f;
		float Ks = 0.4f;
		float Ka = 0.2f;
		float shininess = 500.0f;
	};
	TerrainMat terrainMaterial;
private:
	void ComputeMesh();
	void SetupBuffers();
	void ComputeNormals();
	struct TerrainVertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 UVCoords;
	};
	std::vector<TerrainVertex> vertices;
	std::vector<int> indices;
	unsigned int VBO, VAO, EBO;
};

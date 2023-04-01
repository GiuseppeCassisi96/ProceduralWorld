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

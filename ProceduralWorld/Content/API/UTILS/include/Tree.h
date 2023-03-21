#pragma once
#include <vector>
#include <GLM/include/fwd.hpp>

#include "Model.h"
#include "Shader.h"
#include "Terrain.h"

class Tree
{
	
public:
	Tree(int iterations,std::vector < glm::mat4>& treePos, HeightMap& ElevationMap, HeightMap& BiomeMap);
	void FillTreeArray(std::vector<glm::mat4>& treePos);
	void DrawTrees(glm::mat4 cameraView, 
		Shader& treeShader, Model& tree);
private:
	int iterations;
	HeightMap& ElevationMap;
	HeightMap& BiomeMap;
	glm::vec3 GenerateRandPoint(int randCol, int randRow);
	
};

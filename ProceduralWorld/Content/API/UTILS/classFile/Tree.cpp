#include "UTILS/include/Tree.h"



Tree::Tree(int iterations, std::vector < glm::mat4>& treePos, HeightMap& ElevationMap, HeightMap& BiomeMap)
	: iterations{ iterations}, ElevationMap{ElevationMap}, BiomeMap{BiomeMap}
{
	FillTreeArray(treePos);
}


void Tree::FillTreeArray(std::vector<glm::mat4>& treePos)
{
	int index = 0;
	while(index < iterations)
	{
		int randCol = rand() % MAP_RESOLUTION;
		int randRow = rand() % MAP_RESOLUTION;
		float biomeValue = BiomeMap.At(randRow, randCol);
		const glm::vec3 newPoint = GenerateRandPoint(randCol, randRow);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, newPoint);
		model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.05f));
		treePos[index] = model;
		index++;
	}
}

void Tree::DrawTrees(glm::mat4 cameraView, Shader& treeShader
	, Model& tree)
{
	treeShader.UseProgram();
	treeShader.SetUniformMatrix4("view", cameraView);
	tree.DrawModel();
	
}

glm::vec3 Tree::GenerateRandPoint(int randCol, int randRow)
{
	float Xpos = static_cast<float>(randCol);
	float Zpos = static_cast<float>(randRow);
	float Ypos = ElevationMap.At(randRow, randCol);

	const float newX = Xpos * XDIM;
	const float newY = Ypos * HEIGHT_SCALE;
	const float newZ = Zpos * ZDIM;
	return {newX, newY, newZ};
}


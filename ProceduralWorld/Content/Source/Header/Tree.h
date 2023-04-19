#pragma once
#include "Model.h"
#include "WorldGeneration.h"

class Tree : public Model
{
public:
	Tree(const std::string& path, std::vector<glm::mat4> treePositions);
	void DrawTree();
	void RecomputeTree(std::vector<glm::mat4>& treePositions);
	struct TreeMat
	{
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 albedo = glm::vec3(0.0f, 1.0f, 0.0f);
		float Kd = 0.5f;
		float Ks = 0.4f;
		float Ka = 0.1f;
		float shininess = 5000.0f;
	};
	TreeMat treeMaterial;
private:
	std::vector<glm::mat4> treePositions;
};



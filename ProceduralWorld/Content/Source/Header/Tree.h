#pragma once
#include "Model.h"
#include "WorldGeneration.h"

//The class Tree IS A child of Model (IS A relationship). 
class Tree : public Model
{
public:
	Tree(const std::string& path, std::vector<glm::mat4> treePositions);
	void DrawTree();
	/**
	 * \brief Will call the 'RecomputeModel' method of Model class
	 * \param treePositions Vector of glm::mat4 that contains the model matrices of trees 
	 */
	void RecomputeTree(std::vector<glm::mat4>& treePositions);

	/**
	 * \brief Material of my tree
	 */
	struct TreeMat
	{
		glm::vec3 specularColor = glm::vec3(1.0f, 1.0f, 1.0f);
		float Kd = 0.5f;
		float Ks = 0.4f;
		float Ka = 0.1f;
		float shininess = 5000.0f;
	};
	TreeMat treeMaterial;
private:
	/**
	 * \brief Vector of glm::mat4 that contains the model matrices of trees 
	 */
	std::vector<glm::mat4> treePositions;
};



#include "Tree.h"


Tree::Tree(const std::string& path, std::vector<glm::mat4> treePositions, std::vector<glm::mat3> treeNormalMat)
: Model(path, treePositions, treeNormalMat){}

void Tree::DrawTree()
{
	DrawModel();
}

void Tree::RecomputeTree(std::vector<glm::mat4>& treePositions, std::vector<glm::mat3>& treeNormalMat)
{
	RecomputeModel(treePositions, treeNormalMat);
}



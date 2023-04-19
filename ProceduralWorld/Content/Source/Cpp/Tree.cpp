#include "Tree.h"


Tree::Tree(const std::string& path, std::vector<glm::mat4> treePositions)
: Model(path, treePositions){}

void Tree::DrawTree()
{
	DrawModel();
}

void Tree::RecomputeTree(std::vector<glm::mat4>& treePositions)
{
	RecomputeModel(treePositions);
}



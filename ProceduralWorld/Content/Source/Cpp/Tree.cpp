#include "Tree.h"


Tree::Tree(const std::string& path) : Model(path){}

void Tree::DrawTree()
{
	DrawModel();
}

void Tree::SetupTreePositions(TerrainGeneration& terrainData, int numberOfTree, HeightMap& TreeMap, float thresholdTreeValue)
{
	int count = 0;
	while (count < numberOfTree)
	{
		const int xRandIndex = rand() % MAP_RESOLUTION;
		const int yRandIndex = rand() % MAP_RESOLUTION;
		if(TreeMap.At(yRandIndex, xRandIndex) >= thresholdTreeValue)
		{
			const glm::vec3 treePosition = terrainData.vertices[yRandIndex * MAP_RESOLUTION + xRandIndex].Position;
			treePositions.emplace_back(treePosition);
		}
		count++;
		
	}
}

#pragma once
#include <vector>


class HeightMap
{
	std::vector<float> data;
	int width, height;
public:

	//Rule of 5 
	HeightMap(int width, int height);
	//I delete the copy to avoid new memory allocation.
	//The memory allocation is a huge activity, especially for big vectors
	HeightMap(const HeightMap& copyMap) = delete;
	HeightMap& operator=(const HeightMap& copyMap) = delete;
	//The move semantics simply move the ownership (of a pointer) from a vector to another 
	HeightMap(HeightMap&& otherMap) = default;
	HeightMap& operator=(HeightMap&& otherMap) noexcept = default;
	//The destructor frees all allocated memory
	~HeightMap() = default;
	std::vector<float>& GetData();
	float At(int rowIndex, int colIndex);
};



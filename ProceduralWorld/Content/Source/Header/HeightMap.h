#pragma once
#include <vector>

/**
 * \brief Is a class used to store height data and access them 
 */
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

	/*The move semantics simply move the ownership from a the new instance to another.
	 *This means that the pointer point to new obj*/
	HeightMap(HeightMap&& otherMap) = default;
	HeightMap& operator=(HeightMap&& otherMap) noexcept = default;

	//The destructor frees all allocated memory
	~HeightMap() = default;
	/**
	 * \brief Returns the data of height map
	 * \return return the address of height map data 
	 */
	std::vector<float>& GetData();
	float At(int rowIndex, int colIndex);
};

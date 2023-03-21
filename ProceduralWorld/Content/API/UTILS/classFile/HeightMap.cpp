#include "UTILS/include/HeightMap.h"

#include <iostream>


HeightMap::HeightMap(int width, int height) : width{ width }, height{height}, data( width * height)
{}


std::vector<float>& HeightMap::GetData()
{
	return data;
}


float HeightMap::At(int rowIndex, int colIndex)
{
	return data[colIndex + rowIndex * width];
}


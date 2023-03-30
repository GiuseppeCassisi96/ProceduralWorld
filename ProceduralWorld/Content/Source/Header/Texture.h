#pragma once
#include "GLAD/Include/glad.h"
#include "GLFW/Include/glfw3.h"

#include "HeightMap.h"

class Texture
{
public:
	/**
	 * \brief Create an empty texture
	 */
	Texture(GLenum textureEnum, int textureWidth, int textureHeight);
	void GetValuesFromTexture(std::vector<float>& fData);
	void BindTexture(GLenum textureNum);
private:
	unsigned int textureID;
};


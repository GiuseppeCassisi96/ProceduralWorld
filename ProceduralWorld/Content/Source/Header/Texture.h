#pragma once
#include "GLAD/Include/glad.h"
#include "GLFW/Include/glfw3.h"
#include "STB_IMAGE/Include/stb_image.h"

#include "HeightMap.h"

class Texture
{
public:
	/**
	 * \brief Create an empty texture
	 */
	Texture(GLenum textureEnum, int textureWidth, int textureHeight);
	//Texture file 
	Texture(const char* fileName, GLenum textureEnum);
	void GetValuesFromTexture(std::vector<float>& fData);
	void BindTexture(GLenum textureNum);
	unsigned int textureID;
private:
	int TexWidth, TexHeight;
};


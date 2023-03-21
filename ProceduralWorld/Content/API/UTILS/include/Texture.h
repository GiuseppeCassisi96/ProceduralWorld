#pragma once
#define GLEW_STATIC
#include <vector>
#include <GLEW/Include/GL/glew.h>
#include <STB_IMAGE/Include/stb_image.h>
#include "UTILS/include/Constants.h"
#include <iostream>
#include <GLM/include/vec3.hpp>

#include "HeightMap.h"


class Texture
{
public:
	//Texture file 
	Texture(const char* fileName, GLenum textureEnum);
	//Empty texture
	Texture(GLenum textureEnum);
	Texture(GLenum textureEnum, std::vector<glm::vec3> normalMap);
	void GetValuesOfMap(std::vector<float>& fData);
	void BindTexture(GLenum textureNum);
	void UnbindTexture();
	void DeleteTexture();
	int TexWidth, TexHeight;
	unsigned int textureID;
};

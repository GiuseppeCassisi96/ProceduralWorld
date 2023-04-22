#pragma once
#include "GLAD/Include/glad.h"
#include "GLFW/Include/glfw3.h"
#include "STB_IMAGE/Include/stb_image.h"

#include "HeightMap.h"

class Texture
{
public:
	/**
	 * \brief Creates an empty texture
	 * \param textureUnit Is the texture unit associated with my texture
	 * \param textureWidth The width of the texture (X-Resolution)
	 * \param textureHeight The height of the texture (Y-Resolution)
	 */
	Texture(GLenum textureUnit, int textureWidth, int textureHeight);
	/**
	 * \brief Loads a texture from a file
	 * \param fileName File path
	 * \param textureUnit Is the texture unit associated with my texture
	 */
	Texture(const char* fileName, GLenum textureUnit);
	/**
	 * \brief Get values from a texture and store them in fData
	 * \param fData Is a vector of float that will contains my texture data
	 */
	void GetValuesFromTexture(std::vector<float>& fData);
	void BindTexture(GLenum textureNum);
	unsigned int textureID;
private:
	int TexWidth, TexHeight;
};


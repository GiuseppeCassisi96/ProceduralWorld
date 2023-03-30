#include "Texture.h"




Texture::Texture(GLenum textureEnum, int textureWidth, int textureHeight)
{
	// generate texture
	glGenTextures(1, &textureID);
	glActiveTexture(textureEnum);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// create empty texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, textureWidth, textureHeight, 0, GL_RED, GL_FLOAT, NULL);
	glBindImageTexture(0, textureID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::GetValuesFromTexture(std::vector<float>& fData)
{
	glGetTexImage(GL_TEXTURE_2D, 0,
		GL_RED, GL_FLOAT, fData.data());
}

void Texture::BindTexture(GLenum textureNum)
{
	glActiveTexture(textureNum);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

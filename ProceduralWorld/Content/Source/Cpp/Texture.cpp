#include "Texture.h"

#include <iostream>




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

Texture::Texture(const char* fileName, GLenum textureEnum)
{
	/*I set the view option (Repeat, Clamp border, ecc...) of each axe (S and T -> U and V)
	 * Target: specify the type of texture, we have a 2D texture so we put GL_TEXTURE_2D*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	/*I set the filter for magnifying and minifying*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	/*I load the texture, the weight, TexHeight and number of channels are overwritten in
	 *'TexWidth', 'TexHeight' and 'NOChannels' vars*/
	int NOChannels;
	unsigned char* data = stbi_load(fileName, &TexWidth, &TexHeight, &NOChannels, 0);
	if (data)
	{
		glGenTextures(1, &textureID);
		glActiveTexture(textureEnum);
		//We bind the 'textureID' with textureEnum 
		glBindTexture(GL_TEXTURE_2D, textureID);

		/*Specify the settings for the two-dimensional texture loaded before
		 *level: indicates the LoD used for the texture
		 *internalFormat: specifies the number of color components in the texture
		 *format: specifies the format of the texture data
		 *border: this value must be 0
		 *type: the type of data
		 */
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, TexWidth, TexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		//This will automatically generate all the required mipmaps for the currently bound texture
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_set_flip_vertically_on_load(true);
		glBindTexture(GL_TEXTURE_2D, 0);

	}
	else
	{
		std::cout << "Failed to load texture" << "\n";
	}
	stbi_image_free(data);
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

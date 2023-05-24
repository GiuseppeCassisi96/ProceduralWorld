#include "SkyBox.h"

SkyBox::SkyBox(std::string RootDir)
{
	TexturesFacesDirs = {
		RootDir + "right.jpg",
		RootDir + "left.jpg",
		RootDir + "top.jpg",
		RootDir + "bottom.jpg",
		RootDir + "back.jpg",
		RootDir + "front.jpg"
	};
	SetupBuffer();
	CreateTexture();
}

void SkyBox::SetupBuffer()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void SkyBox::CreateTexture()
{
	glGenTextures(1, &skyBoxTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
	
	// Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(TexturesFacesDirs[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			/*This because, unlike most textures in OpenGL, cubemaps are expected to start in the top
			 * left corner, not the bottom left corner
			 */
			stbi_set_flip_vertically_on_load(false);
			/*Notice that I use 'GL_TEXTURE_CUBE_MAP_POSITIVE_X + i'. This represents the side of the cube that
			 *I currently assigning a texture to. I'm adding with 'i' to it in order to cycle through all
			 *the sides
			 */
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,0,GL_RGB,width,height,0,
				GL_RGB,GL_UNSIGNED_BYTE,data);
			/*The cubemaps works in a left-handed system, while OpenGL works in a right-handed
			 * system. In the case of cubemaps the front is positive z-direction 
			 */
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Failed to load texture: " << TexturesFacesDirs[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void SkyBox::DrawSkyBox(GLenum TextureUnit)
{
	glBindVertexArray(VAO);
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxTexture);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}



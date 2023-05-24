#pragma once
#include <string>
#include <vector>
#include <Include/glad.h>
#include <iostream>
#include <ostream>
#include <STB_IMAGE/Include/stb_image.h>

class SkyBox
{
public:
	SkyBox(std::string RootDir);
	void DrawSkyBox(GLenum TextureUnit);
	/**
	 * \brief A vector that contains the directories of my textures
	 */
	std::vector<std::string> TexturesFacesDirs;
private:
	/**
	* \brief Setups the buffers of my cube
	*/
	void SetupBuffer();
	/**
	 * \brief Create the SkyBox texture obj and loads the six textures of my cubemap. Apply filterings
	 * at every texture
	 */
	void CreateTexture();
	unsigned int VBO, VAO, EBO;
	unsigned int skyBoxTexture;
	/**
	 * \brief Vertices of box
	 */
	const float vertices[24] = {
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
	};

	const int indices[36] = {
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};

};
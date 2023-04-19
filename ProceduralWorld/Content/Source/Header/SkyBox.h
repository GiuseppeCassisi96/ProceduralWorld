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
	void SetupBuffer();
	void CreateTexture(GLenum TextureUnit);
	void DrawSkyBox(GLenum TextureUnit);
	std::vector<std::string> TexturesFacesDirs;
private:
	unsigned int VBO, VAO, EBO;
	unsigned int skyBoxTexture;
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
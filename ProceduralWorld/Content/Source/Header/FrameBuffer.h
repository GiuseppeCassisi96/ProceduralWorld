#pragma once
#include "GLAD/Include/glad.h"
#include "Constants.h"
#include <cstddef>
class FrameBuffer
{
public:
	FrameBuffer(GLenum textureUnit);
	void BindFrameBuffer();
	void UNBindFrameBuffer();
	void DrawFrameBuffer();
	float FBORectangleVertices[24] =
	{
		// Coords    // texCoords
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f,  0.0f, 1.0f
	};
private:
	void SetupRectangleBuffers();
	unsigned int FBO, RBO, rectVAO, rectVBO;
	unsigned int frameBufferTexture;
};

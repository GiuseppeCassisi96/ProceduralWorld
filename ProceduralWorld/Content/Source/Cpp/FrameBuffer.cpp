#include "FrameBuffer.h"

#include <iostream>


FrameBuffer::FrameBuffer(GLenum textureUnit)
{
	//I generate the framebuffer obj
	glGenFramebuffers(1, &FBO);
	//I bind the framebuffer obj. The GL_FRAMEBUFFER target allows both read and write operations
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	/*Texture generation:
	 * I create a texture in order to store the render output inside the texture.
	 * The texture can easily manipulated inside the fragment shader in order to
	 * create post-processing effects
	 */
	glGenTextures(1, &frameBufferTexture);
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, static_cast<GLsizei>(WIDTH), static_cast<GLsizei>(HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//Now I attach the texture to the framebuffer
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferTexture, 0);

	/*Renderbuffer generation:
	 *The renderbuffer can't be read from a shader but is faster than texture. So, because
	 *I won't modify the depth buffer and stencil buffer, I store them to the renderbuffer 
	 */
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	//I configure the renderbuffer storage to store both depth buffer and stencil buffer
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, static_cast<GLsizei>(WIDTH), static_cast<GLsizei>(HEIGHT));
	//Now I attach the stencil buffer and depth buffer to the framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

	//Now I check if all works fine 
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer error: " << fboStatus << "\n";
	}
	else
	{
		SetupRectangleBuffers();
	}
}

void FrameBuffer::BindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void FrameBuffer::UNBindFrameBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::DrawFrameBuffer()
{
	//I draw the framebuffer rectangle
	glBindVertexArray(rectVAO);
	glDisable(GL_DEPTH_TEST); // prevents framebuffer rectangle from being discarded
	glBindTexture(GL_TEXTURE_2D, frameBufferTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::DeleteBuffers()
{
	glDeleteBuffers(1, &rectVBO);
	glDeleteVertexArrays(1, &rectVAO);
	glDeleteFramebuffers(1, &FBO);
	glDeleteRenderbuffers(1, &RBO);
}

void FrameBuffer::SetupRectangleBuffers()
{
	//Prepare framebuffer rectangle VBO and VAO
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	//I fill the buffer data with FBORectangleVertices
	glBufferData(GL_ARRAY_BUFFER, sizeof(FBORectangleVertices), &FBORectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

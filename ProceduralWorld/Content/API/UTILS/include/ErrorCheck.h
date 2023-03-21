#pragma once
#define ASSERT(x) if(!(x)) __debugbreak()
#define GLDebugError(x) GLClearError();\
    x;\
	ASSERT(GLLogErrors(#x, __LINE__));
#include <GLEW/Include/GL/glew.h>
#include <iostream>


//Clears all the error messages
static void GLClearError()
{
    //Loops until we haven't errors
    while (glGetError() != GL_NO_ERROR);
}

//Prints all the errors that occurs
static bool GLLogErrors(const char* functionName, int line)
{
    //Loops until the 'error' var is zero. Prints all possible error for a function
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL ERROR]: \nError code: " << error << "\nFunction name: " << functionName
            << "\nLine: " << line  <<"\n";
        return false;
    }
    return true;
}
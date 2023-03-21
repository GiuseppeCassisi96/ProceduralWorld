#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <GLM/include/gtc/type_ptr.hpp>
#include <fstream>
#define GLEW_STATIC
#include <GLEW/Include/GL/glew.h>

class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	Shader(const char* vertexShaderPath, const char* tcsShaderPath, const char* tesShaderPath, const char* fragmentShaderPath);
	Shader(const char* computeShaderPath);
	void DeleteShaders();
	void DeleteProgram() const;
	void UseProgram() const;
	void SetUniformMatrix4(const char* uniformParamName, const glm::mat4& value) const;
	void SetUniformMatrix3(const char* uniformParamName, const glm::mat3& value) const;
	void SetUniformFloat(const char* uniformParamName, float value) const;
	void SetUniformInt(const char* uniformParamName, int value) const;
	void SetSubroutine(const char* functionName, GLenum shaderType) const;
	void SetUniformVec3(const char* uniformParamName, const glm::vec3& value) const;
	void DispatchCompute();
private:
	unsigned int program;
	unsigned int vertexObj, fragmentObj, computeObj, tcsObj, tesObj;
	std::stringstream vertexStream, fragmentStream, computeStream, tcsStream, tesStream;
	std::string vertexShaderCode, fragmentShaderCode, computeShaderCode, tcsShaderCode, tesShaderCode;
	void CompileShader(const char* shaderName, unsigned int &shaderObj,
		unsigned int shaderType);
};

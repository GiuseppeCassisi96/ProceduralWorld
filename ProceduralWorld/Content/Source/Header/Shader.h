#pragma once
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <GLM/include/gtc/type_ptr.hpp>
#include "GLAD/Include/glad.h"
class Shader
{
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void DeleteShaders();
	void DeleteProgram() const;
	void UseProgram() const;
	void SetUniformMatrix4(const char* uniformParamName, const glm::mat4& value) const;
	void SetUniformMatrix3(const char* uniformParamName, const glm::mat3& value) const;
	void SetUniformFloat(const char* uniformParamName, float value) const;
	void SetUniformInt(const char* uniformParamName, int value) const;
	void SetUniformVec3(const char* uniformParamName, const glm::vec3& value) const;
private:
	unsigned int program;
	unsigned int vertexObj, fragmentObj;
	std::stringstream vertexStream, fragmentStream;
	std::string vertexShaderCode, fragmentShaderCode;
	void CompileShader(const char* shaderName, unsigned int& shaderObj,
		unsigned int shaderType);
};

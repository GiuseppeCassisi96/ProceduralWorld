#pragma once
#include "Constants.h"
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
	Shader(const char* computeShaderPath);
	void DispatchCompute();
	/**
	 * \brief Delete all shader objects
	 */
	void DeleteShaders() const;

	/**
	 * \brief Delete shader program
	 */
	void DeleteProgram() const;

	/**
	 * \brief Use shader program
	 */
	void UseProgram() const;

	/**
	 * \brief Sets an uniform var with name 'uniformParamName' to value 'value'.
	 * \param uniformParamName the name of the uniform var
	 * \param value the value that I want set to the uniform var
	 */
	void SetUniformMatrix4(const char* uniformParamName, const glm::mat4& value) const;
	/**
	 * \brief Sets an uniform var with name 'uniformParamName' to value 'value'.
	 * \param uniformParamName the name of the uniform var
	 * \param value the value that I want set to the uniform var
	 */
	void SetUniformMatrix3(const char* uniformParamName, const glm::mat3& value) const;
	/**
	 * \brief Sets an uniform var with name 'uniformParamName' to value 'value'.
	 * \param uniformParamName the name of the uniform var
	 * \param value the value that I want set to the uniform var
	 */
	void SetUniformFloat(const char* uniformParamName, float value) const;
	/**
	 * \brief Sets an uniform var with name 'uniformParamName' to value 'value'.
	 * \param uniformParamName the name of the uniform var
	 * \param value the value that I want set to the uniform var
	 */
	void SetUniformInt(const char* uniformParamName, int value) const;
	/**
	 * \brief Sets an uniform var with name 'uniformParamName' to value 'value'.
	 * \param uniformParamName the name of the uniform var
	 * \param value the value that I want set to the uniform var
	 */
	void SetUniformVec3(const char* uniformParamName, const glm::vec3& value) const;

	void SetUniformBool(const char* uniformParamName, const bool value) const;

	inline unsigned int GetProgram()
	{
		return program;
	}
private:
	/**
	 * \brief Compile the shader code. If the compilation fail print a message
	 */
	void CompileShader(const char* shaderCode, unsigned int& shaderObj,
		unsigned int shaderType);
	unsigned int program;
	unsigned int vertexObj, fragmentObj, computeObj;
	std::stringstream vertexStream, fragmentStream, computeStream;
	std::string vertexShaderCode, fragmentShaderCode, computeShaderCode;
};

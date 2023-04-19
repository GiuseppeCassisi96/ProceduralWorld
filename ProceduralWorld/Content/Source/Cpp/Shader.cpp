#include "Shader.h"



Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath)
{

	std::ifstream vertexStreamFile, fragmentStreamFile;
	vertexStreamFile.exceptions(std::ios::failbit | std::ios::badbit);
	fragmentStreamFile.exceptions(std::ios::failbit | std::ios::badbit);
	try
	{
		//Open files
		vertexStreamFile.open(vertexShaderPath);
		fragmentStreamFile.open(fragmentShaderPath);
		//Read the buffer stream and get the pointer to the file buffer (the content of the file);
		vertexStream << vertexStreamFile.rdbuf();
		fragmentStream << fragmentStreamFile.rdbuf();
		//Convert the streamstring to string
		vertexShaderCode = vertexStream.str();
		fragmentShaderCode = fragmentStream.str();
		//Close files
		vertexStreamFile.close();
		fragmentStreamFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "file non letto" << "\n";
	}

	//Create a program, that will contains our shader programs, with a program ID
	program = glCreateProgram();
	//Compile the shaders
	CompileShader(vertexShaderCode.c_str(), vertexObj
		, GL_VERTEX_SHADER);
	CompileShader(fragmentShaderCode.c_str(), fragmentObj,
		GL_FRAGMENT_SHADER);
	//Attaches a shader object to a program object
	glAttachShader(program, vertexObj);
	glAttachShader(program, fragmentObj);
	//Links the program. Shader objects are used to create an executable that will run on the programmable shader
	glLinkProgram(program);
	DeleteShaders();
}

Shader::Shader(const char* computeShaderPath)
{
	std::ifstream computeStreamFile;
	computeStreamFile.exceptions(std::ios::failbit | std::ios::badbit);
	try
	{
		//Open file
		computeStreamFile.open(computeShaderPath);
		//Read the buffer stream and get the pointer to the file buffer (the content of the file);
		computeStream << computeStreamFile.rdbuf();
		//Convert the streamstring to string
		computeShaderCode = computeStream.str();
		//Close file
		computeStreamFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "file non letto" << "\n";
	}

	//Create a program, that will contains our shader programs, with a program ID
	program = glCreateProgram();
	//Compile the shaders
	CompileShader(computeShaderCode.c_str(), computeObj
		, GL_COMPUTE_SHADER);
	//Attaches a shader object to a program object
	glAttachShader(program, computeObj);
	//Links the program. Shader objects are used to create an executable that will run on the programmable shader
	glLinkProgram(program);
	glDeleteShader(computeObj);
}

void Shader::DispatchCompute()
{
	glDispatchCompute(MAP_RESOLUTION, MAP_RESOLUTION, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Shader::DeleteShaders() const
{
	glDeleteShader(vertexObj);
	glDeleteShader(fragmentObj);
}

void Shader::DeleteProgram() const
{
	glDeleteProgram(program);
}


void Shader::UseProgram() const
{
	glUseProgram(program);
}

void Shader::SetUniformMatrix4(const char* uniformParamName, const glm::mat4& value) const
{
	const int uniformParamLoc = glGetUniformLocation(program, uniformParamName);
	glUniformMatrix4fv(uniformParamLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniformMatrix3(const char* uniformParamName, const glm::mat3& value) const
{
	const int uniformParamLoc = glGetUniformLocation(program, uniformParamName);
	glUniformMatrix3fv(uniformParamLoc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetUniformFloat(const char* uniformParamName, float value) const
{
	const int uniformParamLoc = glGetUniformLocation(program, uniformParamName);
	glUniform1f(uniformParamLoc, value);
}

void Shader::SetUniformInt(const char* uniformParamName, int value) const
{
	const int uniformParamLoc = glGetUniformLocation(program, uniformParamName);
	glUniform1i(uniformParamLoc, value);
}


void Shader::SetUniformVec3(const char* uniformParamName, const glm::vec3& value) const
{
	const int uniformParamLoc = glGetUniformLocation(program, uniformParamName);
	glUniform3fv(uniformParamLoc, 1, glm::value_ptr(value));
}

void Shader::CompileShader(const char* shaderName, unsigned int& shaderObj,
                           unsigned int shaderType)
{
	//Creates shader reference objects
	shaderObj = glCreateShader(shaderType);
	//Attach the shader source code to the shaderObj
	glShaderSource(shaderObj, 1, &shaderName, nullptr);
	//Compile !
	glCompileShader(shaderObj);

	//Check compilation status 
	int result;
	glGetShaderiv(shaderObj, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int errorLogLenght;
		glGetShaderiv(shaderObj, GL_INFO_LOG_LENGTH, &errorLogLenght);
		//Allocate memory on the stack dynamically 
		char* message = static_cast<char*>(alloca(errorLogLenght * sizeof(char)));
		glGetShaderInfoLog(shaderObj, errorLogLenght, &errorLogLenght, message);
		std::cout << "Failed to compile! \n";
		std::cout << message << "\n";
	}
}

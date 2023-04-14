#pragma once
#include <GLM/include/glm.hpp>
#include <GLM/include/fwd.hpp>
#include <Include/glad.h>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 UVCoord;
	glm::vec3 Normals;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;
	glm::vec3 Color;

};
class Mesh
{
	
public:
	//Constructor
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const std::vector<glm::mat4>& meshPositions = std::vector<glm::mat4>{ 0 }, 
		const std::vector<glm::mat3>& meshNormalMat = std::vector<glm::mat3>{ 0 }) noexcept;

	//COPY
	Mesh(const Mesh& mesh) = delete;
	Mesh& operator=(const Mesh& mesh) = delete;

	//MOVE
	Mesh(Mesh&& mesh) noexcept;
	Mesh& operator=(Mesh&& mesh) noexcept;

	//Destructor
	~Mesh() noexcept;


	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;
	void DrawMesh();
	void RecomputeMesh(std::vector<glm::mat4>& meshPositions, std::vector<glm::mat3>& meshNormalMat);
private:
	unsigned int VBO, EBO, VAO, instanceVBO, normalInstanceVBO;
	std::vector<glm::mat4> meshPositions;
	std::vector<glm::mat3> meshNormalMat;
	void FreeGPUResources();
	void SetupMesh();
};
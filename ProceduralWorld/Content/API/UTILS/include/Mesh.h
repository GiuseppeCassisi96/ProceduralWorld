#pragma once
#define GLEW_STATIC
#include <GLM/include/glm.hpp>
#include <vector>
#include <GLEW/Include/GL/glew.h>
#include <GLM/include/fwd.hpp>
#include "UTILS/include/Constants.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 UVCoord;
	glm::vec3 Normals;
	glm::vec3 Tangent;
	glm::vec3 BiTangent;

};

struct TerrainVertex
{
	glm::vec3 Position;
	glm::vec2 UVCoord;
};


class Mesh
{

public:
	//Constructor
	Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
		const std::vector<glm::mat4>& meshPositions = std::vector<glm::mat4>{0}) noexcept;

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
private:
	unsigned int VBO, EBO, VAO, instanceVBO;
	std::vector<glm::mat4> meshPositions;
	void FreeGPUResources();
	void SetupMesh();
};

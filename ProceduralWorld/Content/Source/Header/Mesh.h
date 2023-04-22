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
		const std::vector<glm::mat4>& meshPositions = std::vector<glm::mat4>{ 0 }) noexcept;

	//COPY: I delete the copy semantics to avoid copy
	Mesh(const Mesh& mesh) = delete;
	Mesh& operator=(const Mesh& mesh) = delete;

	//MOVE:
	/**
	 * \brief  Here I change ownership from an instance to another(Move constructor)
               In this case I construct a new instance by moving the resources of one
               instance (fully) to another instance (empty)
	 * \param mesh The fully instance
	 */
	Mesh(Mesh&& mesh) noexcept;
	Mesh& operator=(Mesh&& mesh) noexcept;

	//DESTRUCTOR: Free GPU resources
	~Mesh() noexcept;


	std::vector<Vertex> meshVertices;
	std::vector<unsigned int> meshIndices;
	void DrawMesh();
	/**
	 * \brief Update the buffer of instances with new meshPositions data
	 * \param meshPositions A vector of model matrices 
	 */
	void RecomputeMesh(std::vector<glm::mat4>& meshPositions);
private:
	unsigned int VBO, EBO, VAO, instanceVBO;
	std::vector<glm::mat4> meshPositions;
	/**
	 * \brief Delete all mesh buffers
	 */
	void FreeGPUResources();
	/**
	 * \brief Setup all mesh buffers
	 */
	void SetupMesh();
};
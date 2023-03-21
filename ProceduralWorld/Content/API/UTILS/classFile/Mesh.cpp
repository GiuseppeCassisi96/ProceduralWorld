#include "UTILS/include/Mesh.h"


// We use initializer list and std::move in order to avoid a copy of the arguments
Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, 
const std::vector<glm::mat4>& meshPositions) noexcept :
meshVertices{ std::move(vertices)},meshIndices{ std::move(indices) },
meshPositions{meshPositions}
{
	SetupMesh();
}

/*Here I change ownership from an instance to another(Move constructor)
 * In this case I construct a new instance by moving the resources of one
 * instance (fully) to another instance (empty)
 */
Mesh::Mesh(Mesh&& mesh) noexcept :
meshVertices{ std::move(mesh.meshVertices) }, meshIndices{ std::move(mesh.meshIndices) },
VAO{ mesh.VAO }, VBO{ mesh.VBO }, EBO{ mesh.EBO }, instanceVBO{mesh.instanceVBO},
meshPositions{ std::move(mesh.meshPositions)}
{
	mesh.VAO = 0;
}
/*(Move assignment) in this case I have two fully instances and I want
 * move the resources of the second to the first*/
Mesh& Mesh::operator=(Mesh&& mesh) noexcept
{
	FreeGPUResources();//I Empty the resources of the first
	//I check if the second one has resources or not 
	if(mesh.VAO)
	{
		/*If the second one has resources I move the resources of the second
		 * to the first (Move ownership)*/
		meshVertices = std::move(mesh.meshVertices);
		meshIndices = std::move(mesh.meshIndices);
		meshPositions = std::move(mesh.meshPositions);
		VAO = mesh.VAO;
		VBO = mesh.VBO;
		instanceVBO = mesh.instanceVBO;
		EBO = mesh.EBO;
		mesh.VAO = 0;
	}
	else
	{
		VAO = 0;
	}
	return  *this;
}


Mesh::~Mesh() noexcept
{
	FreeGPUResources();
}

void Mesh::SetupMesh()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glBufferData(GL_ARRAY_BUFFER, meshVertices.size() * sizeof(Vertex), meshVertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshIndices.size() * sizeof(unsigned int), meshIndices.data(), GL_STATIC_DRAW);

	

	//Here I define how my VBO is structured (offset, type, data structure)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex,UVCoord)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Normals)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, Tangent)));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, BiTangent)));
	glEnableVertexAttribArray(4);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Instancing technique to instance multiple objs in a performant way
	if(!meshPositions.empty())
	{
		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, meshPositions.size() * sizeof(glm::mat4),
			meshPositions.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(0));
		glEnableVertexAttribArray(5);

		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
		glEnableVertexAttribArray(6);

		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(7);

		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
		glEnableVertexAttribArray(8);

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}


void Mesh::FreeGPUResources()
{
	if(VAO)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &instanceVBO);
	}
	
}

void Mesh::DrawMesh()
{
	if(!meshPositions.empty())
	{
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(meshIndices.size()), GL_UNSIGNED_INT, 0, NUMBER_OF_TREE);
		glBindVertexArray(0);
	}
	else
	{
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(meshIndices.size()), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	
}


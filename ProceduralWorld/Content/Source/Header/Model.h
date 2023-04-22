#pragma once
#include <string>
#include <GLM/include/fwd.hpp>
#include "ASSIMP/assimp/scene.h"
#include "ASSIMP/assimp/postprocess.h"
#include "ASSIMP/assimp/Importer.hpp"
#include "Mesh.h"

class Model
{
public:
	std::vector<Mesh> meshes;
	//COPY:
	//Here I'm deleting copy constructor 
	Model(const Model& model) = delete;
	//Here I am prohibiting the copy operation going to deleting the copy operator 
	Model& operator=(const Model& copy) = delete;

	//MOVE
	//internally does a memberwise std::move
	Model(Model&& model) = default;
	Model& operator=(Model&& model) noexcept = default;

	Model(const std::string& path, std::vector<glm::mat4> modelPositions);

	void DrawModel();
	/**
	 * \brief Will call the 'RecomputeMesh' method of Mesh class
	 * \param modelPositions A vector of model matrices
	 */
	void RecomputeModel(std::vector<glm::mat4>& modelPositions);
private:
	/**
	 * \brief loading of the model using Assimp library. Nodes are processed to build a vector of
	 * Mesh class instances. I obtain an 'aiScene' instance that contains nodes of Assimp data structure.
	 * Each node contains a number of mesh
	 * \param path The path of 3D model
	 */
	void LoadModel(const std::string& path);
	/**
	 * \brief Recursive processing of nodes of Assimp data structure
	 * \param node The node that I want process 
	 * \param scene The my whole scene, contains the Assimp data structure
	 */
	void ProcessNode(aiNode* node, const aiScene* scene);
	/**
	 * \brief Here I convert the data structure of assimp mesh to a data structure defined in the
	 * mesh class (aka Vertex).
	 * \param mesh The mesh that I want convert in a "OpenGL mesh"
	 * \return The mesh converted and initialized (I setup all mesh buffers) 
	 */
	Mesh ProcessMesh(aiMesh* mesh);
	std::vector<glm::mat4> modelPositions;
};

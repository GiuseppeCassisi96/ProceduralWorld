#include "UTILS/include/Model.h"
#include <iostream>

Model::Model(const std::string& path, const std::vector<glm::mat4>& modelPositions ) : modelPositions{modelPositions}
{
	LoadModel(path);
}


void Model::LoadModel(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,aiProcess_Triangulate 
		| aiProcess_JoinIdenticalVertices | aiProcess_FlipUVs | aiProcess_GenSmoothNormals
		| aiProcess_CalcTangentSpace);
	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR ASSIMP: " << importer.GetErrorString() << "\n";
		return;
	}
	ProcessNode(scene->mRootNode, scene);
}

/*Here we recursively create and process instances of mesh class and
 *this is the reason why to manage the memory using copy semantics
 *and move semantics 
 */
void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	//I iterate on the meshes inside this node
	for(unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//scene->mMeshes stores an array of meshes
		//node->mMeshes stores indexes of the meshes stored into scene->mMeshes
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.emplace_back(ProcessMesh(mesh));
	}
	//Each node can have children, so I iterate on the children inside this node
	for(unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

//Here I convert the data structure of assimp mesh to a data structure defined in the mesh class (aka Vertex)
Mesh Model::ProcessMesh(aiMesh* mesh)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	//Here I process the meshVertices
	for(unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//Here I convert to assimp mesh data structure to Vertex data structure (POSITION)
		glm::vec3 position{ mesh->mVertices[i].x,mesh->mVertices[i].y ,mesh->mVertices[i].z};
		vertex.Position = position;
		//Here I convert to assimp mesh data structure to Vertex data structure (NORMALS)
		glm::vec3 normal{ mesh->mNormals[i].x,mesh->mNormals[i].y ,mesh->mNormals[i].z };
		vertex.Normals = normal;
		if(mesh->mTextureCoords[0])
		{
			//Here I convert to assimp mesh data structure to Vertex data structure (UVCOORD)
			glm::vec2 uv{mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
			vertex.UVCoord = uv;
			//Here I convert to assimp mesh data structure to Vertex data structure (TANGENT)
			glm::vec3 tangent{ mesh->mTangents[i].x,mesh->mTangents[i].y ,mesh->mTangents[i].z };
			vertex.Tangent = tangent;
			//Here I convert to assimp mesh data structure to Vertex data structure (BITANGENT)
			glm::vec3 biTangent{ mesh->mBitangents[i].x,mesh->mBitangents[i].y ,mesh->mBitangents[i].z };
			vertex.BiTangent = biTangent;
		}
		else
		{
			glm::vec2 uv{ 0.0f, 0.0f };
			vertex.UVCoord = uv;
			std::cout << "WARNING: mesh doesn't have UVCoord"<<"\n";
		}
		vertices.emplace_back(vertex);
	}
	//Here I process the meshIndices
	for(unsigned int i = 0; i< mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for(unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.emplace_back(face.mIndices[j]);
		}
	}
	if(modelPositions.empty())
	{
		return Mesh(vertices, indices);
	}
	else
	{
		return  Mesh(vertices, indices, modelPositions);
	}
	
}


void Model::DrawModel()
{
	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].DrawMesh();
	}
}


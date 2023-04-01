#include "WorldGeneration.h"

#include <GLM/include/ext/quaternion_geometric.hpp>


TerrainGeneration::TerrainGeneration(HeightMap& ElevationMap) : ElevationMap(std::move(ElevationMap))
{
    ComputeMesh();
    SetupBuffers();
}

void TerrainGeneration::ComputeMesh()
{
    for (int i = 0; i < MAP_RESOLUTION; i++)
    {
        for (int j = 0; j < MAP_RESOLUTION; j++)
        {
            const auto width = static_cast<float>(MAP_RESOLUTION);
            const auto height = static_cast<float>(MAP_RESOLUTION);
            const auto fi = static_cast<float>(i);
            const auto fj = static_cast<float>(j);
            const auto fMeshResolution = static_cast<float>(MAP_RESOLUTION);
            TerrainVertex vertex;
            vertex.Position = glm::vec3
            (
                -width / 2.0f + width * fi / fMeshResolution //X
                , ElevationMap.At(i, j) * HEIGHT_SCALE //Y
                , -height / 2.0f + height * fj / fMeshResolution //Z
            );
            vertex.UVCoords = glm::vec2
            (
                fi / fMeshResolution, //U
                fj / fMeshResolution  //V
            );
            vertex.Normal = glm::vec3(0.0f);
            vertices.push_back(vertex);
        }
        
    }
    

    int rowOffset = 0;
    for (int i = 0; i < MAP_RESOLUTION - 1; i++)
    {
        for (int j = 0; j < MAP_RESOLUTION - 1; j++)
        {
            //First triangle
            indices.emplace_back(j + rowOffset + MAP_RESOLUTION);
            indices.emplace_back(j + rowOffset);
            indices.emplace_back(j + 1 + rowOffset);


            //Second triangle
            indices.emplace_back(j + 1 + rowOffset);
            indices.emplace_back(j + rowOffset + MAP_RESOLUTION + 1);
            indices.emplace_back(j + rowOffset + MAP_RESOLUTION);

        }
        rowOffset += MAP_RESOLUTION;
    }
    ComputeNormals();
}

void TerrainGeneration::SetupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void TerrainGeneration::ComputeNormals()
{
    for(int i = 0; i < MAP_RESOLUTION - 1; i++)
    {
	    for(int j = 0; j < MAP_RESOLUTION - 1; j++)
	    {
            const int index = i * MAP_RESOLUTION + j;
            glm::vec3 firstEdge = vertices[index + 1].Position - vertices[index].Position;
            glm::vec3 secondEdge = vertices[index + MAP_RESOLUTION].Position - vertices[index].Position;
            vertices[index].Normal = glm::normalize(glm::cross(firstEdge, secondEdge));
	    }
    }
    for(int i = 0; i < MAP_RESOLUTION - 1; i++)
    {
        const int index = i * MAP_RESOLUTION + (MAP_RESOLUTION - 1);
        glm::vec3 firstEdge = vertices[index - 1].Position - vertices[index].Position;
        glm::vec3 secondEdge = vertices[index + MAP_RESOLUTION].Position - vertices[index].Position;
        vertices[index].Normal = glm::normalize(glm::cross(secondEdge, firstEdge));
    }

    for (int j = 0; j < MAP_RESOLUTION - 1; j++)
    {
        const int index = (MAP_RESOLUTION - 1) * MAP_RESOLUTION + j;
        glm::vec3 firstEdge = vertices[index + 1].Position - vertices[index].Position;
        glm::vec3 secondEdge = vertices[index - MAP_RESOLUTION].Position - vertices[index].Position;
        vertices[index].Normal = glm::normalize(glm::cross(secondEdge, firstEdge));
    }
}

void TerrainGeneration::DrawTerrain()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void TerrainGeneration::ReComputeMesh()
{
    for (int i = 0; i < MAP_RESOLUTION; i++)
    {
        for (int j = 0; j < MAP_RESOLUTION; j++)
        {
            vertices[i * MAP_RESOLUTION + j].Position.y = ElevationMap.At(i, j) * HEIGHT_SCALE;
        }
    }
    ComputeNormals();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_DYNAMIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
}





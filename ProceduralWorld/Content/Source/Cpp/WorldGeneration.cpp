#include "WorldGeneration.h"



TerrainGeneration::TerrainGeneration()
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
                , 0.0f //Y
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
}

void TerrainGeneration::SetupBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

}

void TerrainGeneration::DrawTerrain()
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}





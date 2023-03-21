#include "UTILS/include/Terrain.h"


Terrain::Terrain(int numberOfPatch, HeightMap& ElevationMap) : numberOfPatch{numberOfPatch},
ElevationMap{ElevationMap}
{
    SetupTerrain();
    ComputeNormalMap();
}


void Terrain::SetupTerrain()
{
    const auto fNumOfPatch = static_cast<float>(numberOfPatch);
    const auto fWidth = static_cast<float>(MAP_RESOLUTION);
    const auto fHeight = static_cast<float>(MAP_RESOLUTION);
    
    for (int i = 0; i < numberOfPatch; i++)
    {
        for (int j = 0; j < numberOfPatch; j++)
        {
            const auto fI = static_cast<float>(i);
            const auto fJ = static_cast<float>(j);
            TerrainVertex firstVertex {};
            TerrainVertex secondVertex{};
            TerrainVertex thirdVertex {};
            TerrainVertex fourthVertex {};
            //First vertex
            firstVertex.Position = glm::vec3(
                 fWidth * fI / fNumOfPatch, 
                0.0f, 
                fHeight * fJ / fNumOfPatch);

            firstVertex.UVCoord = glm::vec2(
                fI / fNumOfPatch * NUMBER_OF_TILE,
                fJ / fNumOfPatch * NUMBER_OF_TILE);


            //Second vertex
            secondVertex.Position = glm::vec3(
                fWidth * (fI + 1) / fNumOfPatch,
                0.0f, 
                fHeight * fJ / fNumOfPatch);

            secondVertex.UVCoord = glm::vec2(
                (fI + 1) / fNumOfPatch * NUMBER_OF_TILE,
                fJ / fNumOfPatch * NUMBER_OF_TILE);


            //Third vertex
            thirdVertex.Position = glm::vec3(
                  fWidth * fI / fNumOfPatch,
                0.0f, 
                fHeight * (fJ + 1) / fNumOfPatch);

            thirdVertex.UVCoord = glm::vec2(
                fI / fNumOfPatch * NUMBER_OF_TILE,
                (fJ + 1) / fNumOfPatch * NUMBER_OF_TILE);


            //Fourth vertex
            fourthVertex.Position = glm::vec3(
                fWidth * (fI + 1) / fNumOfPatch,
                0.0f, 
                fHeight * (fJ + 1) / fNumOfPatch);

            fourthVertex.UVCoord = glm::vec2(
                (fI + 1) / fNumOfPatch * NUMBER_OF_TILE,
                (fJ + 1) / fNumOfPatch * NUMBER_OF_TILE);

            vertices.emplace_back(firstVertex);
            vertices.emplace_back(secondVertex);
            vertices.emplace_back(thirdVertex);
            vertices.emplace_back(fourthVertex);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);


    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TerrainVertex), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    glBindVertexArray(0);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
}

void Terrain::ComputeNormalMap()
{
    for (int i = 0; i < MAP_RESOLUTION; i++)
    {
        for (int j = 0; j < MAP_RESOLUTION; j++)
        {
            int upIndex = i - 1;
            int downIndex = i + 1;
            int rightIndex = j + 1;
            int leftIndex = j - 1;
            if (upIndex < 0)
            {
                upIndex %= MAP_RESOLUTION;
                while (upIndex < 0)
                {
                    upIndex += MAP_RESOLUTION;
                }
            }

            if (downIndex >= MAP_RESOLUTION)
            {
                downIndex %= MAP_RESOLUTION;
                while (downIndex < 0)
                {
                    downIndex += MAP_RESOLUTION;
                }
            }

            if(rightIndex >= MAP_RESOLUTION)
            {
                rightIndex %= MAP_RESOLUTION;
                while (rightIndex < 0)
                {
                    rightIndex += MAP_RESOLUTION;
                }
            }

            if(leftIndex < 0)
            {
                leftIndex %= MAP_RESOLUTION;
                while (leftIndex < 0)
                {
                    leftIndex += MAP_RESOLUTION;
                }
            }
            float t = ElevationMap.At(upIndex, j);//TOP
            float tr = ElevationMap.At(upIndex, rightIndex);//TOP-RIGHT
            float tl = ElevationMap.At(upIndex, leftIndex); //TOP-LEFT
            float r = ElevationMap.At(i, rightIndex); //RIGHT
            float l = ElevationMap.At(i, leftIndex); //LEFT
            float b = ElevationMap.At(downIndex, j); //BOTTOM
            float br = ElevationMap.At(downIndex, rightIndex); //BOTTOM
            float bl = ElevationMap.At(downIndex,leftIndex); //BOTTOM
            // sobel filter
            const float dX = (tr + 2.0f * r + br) - (tl + 2.0f * l + bl);
            const float dY = (bl + 2.0f * b + br) - (tl + 2.0f * t + tr);
            const float dZ = 1.0f/9.0f;

            glm::vec3 n(dX, dY, dZ);
            n = glm::normalize(n);
            NormalMap.emplace_back(n);
            
        }
    }

}


void Terrain::DrawTerrain(glm::mat4& terrainModel, glm::mat4& cameraView, Shader& terrainShader)
{
    terrainModel = glm::mat4{ 1.0f };
    terrainModel = glm::translate(terrainModel, glm::vec3(0.0f));
    terrainModel = glm::rotate(terrainModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    terrainModel = glm::scale(terrainModel,  glm::vec3(XDIM, 1.0f, ZDIM));
    

    terrainShader.UseProgram();
    terrainShader.SetUniformMatrix4("model", terrainModel);
    terrainShader.SetUniformMatrix4("view", cameraView);

    glBindVertexArray(VAO);
    glDrawArrays(GL_PATCHES, 0, 4 * numberOfPatch * numberOfPatch);
    glBindVertexArray(0);
}









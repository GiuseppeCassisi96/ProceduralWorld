#include "IMGUI/imgui.h"
#include "IMGUI/imgui_impl_glfw.h"
#include "IMGUI/imgui_impl_opengl3.h"
#include "WorldGeneration.h"
#include "GLAD/Include/glad.h"
#include "GLFW/Include/glfw3.h"
#include "Movement.h"
#include "Shader.h"
#include <iostream>
#include "HeightMap.h"
#include "Texture.h"
#include <GLM/include/glm.hpp>
#include <GLM/include/gtc/matrix_transform.hpp>
#include <GLM/include/gtc/matrix_inverse.hpp>
#include <GLM/include/gtc/type_ptr.hpp>
#include "Tree.h"
#include "SkyBox.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void recompute_heightMap(Shader& NoiseShader, Shader& terrainShader,Texture& NoiseText, Texture& BiomeNoiseText, Texture& TreeNoiseText, TerrainGeneration& terrain);
GLFWwindow* Setup(GLFWwindow* window);
void SetupTreePositions(TerrainGeneration& terrainData, int numberOfIterations, HeightMap& TreeMap, float thresholdTreeValue);

//Global vars
bool isWireframe = false;
bool isMouseVisible = false;
glm::mat4 WorldCamera = glm::mat4(1.0f);
glm::mat4 WorldProjection = glm::perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
    0.1f, 1000000.0f);
Movement playerMovement{ glm::vec3(0.0f, 0.0f, 7.0f), WorldCamera };
std::string shadersPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Cpp/Shaders/";
std::string texturesPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Textures/";
std::string modelsPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Models/";
std::string skyPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Textures/SkyBox/";
float amplitude = 1.7f;
float frequency = 3.5f;
int octaves = 10;
HeightMap ElevationMap(MAP_RESOLUTION, MAP_RESOLUTION);
HeightMap BiomeMap(MAP_RESOLUTION, MAP_RESOLUTION);
HeightMap TreeMap(MAP_RESOLUTION, MAP_RESOLUTION);
unsigned int TerrainSubLocationIndex, ModelSubLocationIndex, SkySubLocationIndex;
unsigned int TerrainSubVertexLoc, ModelSubVertexLoc, SkySubVertexLoc;
std::vector<glm::mat4> treeModels;
//Light
glm::vec3 lightDir = glm::vec3(0.6f, 0.3f, 0.0f);
float lightIntensity = 2.5f;
//Tree
float thresholdTreeValue = 0.5f;
int main()
{
    
    float oldFrequency = frequency;
    float oldAmplitude = amplitude;
    int oldOctaves = octaves;
    float oldThresholdTreeValue = thresholdTreeValue;
    GLFWwindow* window = nullptr;
    window = Setup(window);
    
    //Creation of HeightMap
    Shader NoiseShader{ (shadersPath + "NoiseGeneration.comp").c_str() };
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);
    NoiseShader.SetUniformFloat("seed", 0.0f);

    Texture NoiseText{ GL_TEXTURE0, MAP_RESOLUTION, MAP_RESOLUTION };
    NoiseText.BindTexture(GL_TEXTURE0);
    NoiseShader.DispatchCompute();
    NoiseText.GetValuesFromTexture(ElevationMap.GetData());

    Texture BiomeNoiseText{ GL_TEXTURE1, MAP_RESOLUTION, MAP_RESOLUTION };
    BiomeNoiseText.BindTexture(GL_TEXTURE1);
    NoiseShader.DispatchCompute();
    BiomeNoiseText.GetValuesFromTexture(BiomeMap.GetData());

    Texture TreeNoiseText{ GL_TEXTURE2, MAP_RESOLUTION, MAP_RESOLUTION };
    TreeNoiseText.BindTexture(GL_TEXTURE2);
    NoiseShader.DispatchCompute();
    TreeNoiseText.GetValuesFromTexture(TreeMap.GetData());

    Texture StylizedGrassTexture((texturesPath + "StylizedGrass.jpg").c_str(), GL_TEXTURE3);
    Texture StylizedLeavesTexture((texturesPath + "StylizedLeaves.jpg").c_str(), GL_TEXTURE4);
    Texture StylizedRockTexture((texturesPath + "StylizedRock.jpg").c_str(), GL_TEXTURE5);
    
    StylizedGrassTexture.BindTexture(GL_TEXTURE3);
    StylizedLeavesTexture.BindTexture(GL_TEXTURE4);
    StylizedRockTexture.BindTexture(GL_TEXTURE5);

    Shader terrainShader{ (shadersPath + "TerrainShader.vert").c_str(),
                      (shadersPath + "TerrainShader.frag").c_str() };
    TerrainGeneration terrain (ElevationMap);

	glm::mat4 TerrainModel = glm::mat4(1.0f);
    glm::mat3 TerrainNormalMatrix = glm::mat3(1.0f);

    TerrainModel = glm::translate(TerrainModel, glm::vec3(0.0f));
    TerrainNormalMatrix = glm::inverseTranspose(glm::mat3(TerrainModel));



    TerrainSubLocationIndex = glGetSubroutineIndex(terrainShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForTerrain");
    ModelSubLocationIndex = glGetSubroutineIndex(terrainShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForModels");
	SkySubLocationIndex = glGetSubroutineIndex(terrainShader.GetProgram(), GL_FRAGMENT_SHADER,
        "SkyBoxFrag");

    TerrainSubVertexLoc = glGetSubroutineIndex(terrainShader.GetProgram(), GL_VERTEX_SHADER,
        "TerrainVert");
    ModelSubVertexLoc = glGetSubroutineIndex(terrainShader.GetProgram(), GL_VERTEX_SHADER,
        "TreeVert");
    SkySubVertexLoc = glGetSubroutineIndex(terrainShader.GetProgram(), GL_VERTEX_SHADER,
        "SkyBoxVert");

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubLocationIndex);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &TerrainSubLocationIndex);

    terrainShader.UseProgram();
    terrainShader.SetUniformMatrix4("model", TerrainModel);
    terrainShader.SetUniformMatrix4("view", WorldCamera);
    terrainShader.SetUniformMatrix4("proj", WorldProjection);
    terrainShader.SetUniformVec3("viewPos", playerMovement.position);
    terrainShader.SetUniformVec3("specularColor", terrain.terrainMaterial.specularColor);
    terrainShader.SetUniformVec3("lightDir", lightDir);
    terrainShader.SetUniformFloat("Ka", terrain.terrainMaterial.Ka);
    terrainShader.SetUniformFloat("Ks", terrain.terrainMaterial.Ks);
    terrainShader.SetUniformFloat("Kd", terrain.terrainMaterial.Kd);
    terrainShader.SetUniformFloat("shininess", terrain.terrainMaterial.shininess);
    terrainShader.SetUniformFloat("lightIntensity", lightIntensity);
    terrainShader.SetUniformInt("BiomeMap", 1);
    terrainShader.SetUniformInt("Lawn", terrain.terrainMaterial.Lawn);
    terrainShader.SetUniformInt("Forest", terrain.terrainMaterial.Forest);
    terrainShader.SetUniformInt("Mountain", terrain.terrainMaterial.Mountain);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubLocationIndex);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &ModelSubVertexLoc);

    SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap, thresholdTreeValue);
    Tree TreeModel((modelsPath + "Tree.fbx").c_str(),treeModels);
    terrainShader.SetUniformVec3("specularColor", TreeModel.treeMaterial.specularColor);
    terrainShader.SetUniformVec3("albedo", TreeModel.treeMaterial.albedo);
    terrainShader.SetUniformFloat("Ka", TreeModel.treeMaterial.Ka);
    terrainShader.SetUniformFloat("Ks", TreeModel.treeMaterial.Ks);
    terrainShader.SetUniformFloat("Kd", TreeModel.treeMaterial.Kd);
    terrainShader.SetUniformFloat("shininess", TreeModel.treeMaterial.shininess);
    terrainShader.SetUniformFloat("lightIntensity", lightIntensity);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &SkySubLocationIndex);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &SkySubVertexLoc);
    SkyBox sky(skyPath);
    sky.CreateTexture(GL_TEXTURE6);
    terrainShader.SetUniformInt("skybox", 6);
    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        playerMovement.Move(window, WorldCamera);

        //ImGuI frame creation
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (isWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        terrainShader.UseProgram();
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubLocationIndex);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &TerrainSubVertexLoc);

        terrainShader.SetUniformMatrix3("normalMatrix", TerrainNormalMatrix);
        terrainShader.SetUniformMatrix4("model", TerrainModel);
        terrainShader.SetUniformMatrix4("view", WorldCamera);
        terrainShader.SetUniformVec3("cameraPos", playerMovement.position);
        terrainShader.SetUniformVec3("lightDir", lightDir);
        terrainShader.SetUniformFloat("lightIntensity", lightIntensity);
        terrainShader.SetUniformFloat("Ka", terrain.terrainMaterial.Ka);
        terrainShader.SetUniformFloat("Ks", terrain.terrainMaterial.Ks);
        terrainShader.SetUniformFloat("Kd", terrain.terrainMaterial.Kd);
        terrainShader.SetUniformFloat("shininess", terrain.terrainMaterial.shininess);
        terrainShader.SetUniformVec3("specularColor", terrain.terrainMaterial.specularColor);
        terrain.DrawTerrain();

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubLocationIndex);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &ModelSubVertexLoc);
        terrainShader.SetUniformVec3("specularColor", TreeModel.treeMaterial.specularColor);
        terrainShader.SetUniformVec3("albedo", TreeModel.treeMaterial.albedo);
        terrainShader.SetUniformFloat("Ka", TreeModel.treeMaterial.Ka);
        terrainShader.SetUniformFloat("Ks", TreeModel.treeMaterial.Ks);
        terrainShader.SetUniformFloat("Kd", TreeModel.treeMaterial.Kd);
        terrainShader.SetUniformFloat("shininess", TreeModel.treeMaterial.shininess);
        TreeModel.DrawTree();

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &SkySubLocationIndex);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &SkySubVertexLoc);

        //change depth function so depth test passes when values are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);
        glm::mat4 skyView = glm::mat4(1.0f);
        skyView = glm::mat4(glm::mat3(WorldCamera));
        terrainShader.SetUniformMatrix4("view", skyView);
        sky.DrawSkyBox(GL_TEXTURE6);
        //set depth function back to default
        glDepthFunc(GL_LESS);

        //Setting of whole application UI
        ImGui::Begin("ProceduralWorld control panel ");
        ImGui::Text("Here you can modify all terrain generation parameters");
        ImGui::SliderFloat("Frequency", &frequency, 0.1f, 3.5f);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.1f, 3.0f);
        ImGui::SliderInt("Octaves", &octaves, 2, 10);
        ImGui::SliderFloat("XLightDir", &lightDir.x, -1.0f, 1.0f);
        ImGui::SliderFloat("YLightDir", &lightDir.y, 0.0f, 1.0f);
        ImGui::SliderFloat("LightIntensity", &lightIntensity, 0.0f, 7.0f);
        ImGui::SliderFloat("Threshold tree value", &thresholdTreeValue, -3.0f, 3.0f);
        ImGui::Text("Frame time: %fms", playerMovement.deltaTime);
        ImGui::End();

        //Rendering of application UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(oldThresholdTreeValue != thresholdTreeValue)
        {
            oldThresholdTreeValue = thresholdTreeValue;
            treeModels.clear();
            SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap, thresholdTreeValue);
            TreeModel.RecomputeTree(treeModels);
        }

        if(oldFrequency != frequency || oldAmplitude != amplitude || oldOctaves != octaves)
        {
            //Recompute Terrain
            recompute_heightMap(NoiseShader,terrainShader,NoiseText, BiomeNoiseText, TreeNoiseText,terrain);
            oldFrequency = frequency;
            oldAmplitude = amplitude;
            oldOctaves = octaves;
            terrain.ReComputeMesh();

            //Recompute Tree positions
            treeModels.clear();
            SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap,thresholdTreeValue);
            TreeModel.RecomputeTree(treeModels);
        }

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        isWireframe = !isWireframe;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
        isMouseVisible = !isMouseVisible;
        if(isMouseVisible)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
    }
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
GLFWwindow* Setup(GLFWwindow* window)
{
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    window = glfwCreateWindow(WIDTH, HEIGHT, "ProceduralWorld", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);


    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }
    glEnable(GL_DEPTH_TEST);
    //ImGUI setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsClassic();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    return window;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!isMouseVisible)
		playerMovement.Rotate(xpos, ypos);
}
void recompute_heightMap(Shader& NoiseShader, Shader& terrainShader,Texture& NoiseText, Texture& BiomeNoiseText, Texture& TreeNoiseText, TerrainGeneration& terrain)
{
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);


    NoiseText.BindTexture(GL_TEXTURE0);
    glBindImageTexture(0, NoiseText.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    NoiseShader.DispatchCompute();
    NoiseText.GetValuesFromTexture(terrain.ElevationMap.GetData());

    BiomeNoiseText.BindTexture(GL_TEXTURE1);
    glBindImageTexture(0, BiomeNoiseText.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    NoiseShader.DispatchCompute();
    BiomeNoiseText.GetValuesFromTexture(BiomeMap.GetData());

    TreeNoiseText.BindTexture(GL_TEXTURE2);
    glBindImageTexture(0, TreeNoiseText.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    NoiseShader.DispatchCompute();
    TreeNoiseText.GetValuesFromTexture(TreeMap.GetData());

    terrainShader.UseProgram();
    terrainShader.SetUniformInt("BiomeMap", 1);
}
void SetupTreePositions(TerrainGeneration& terrainData, int numberOfIterations, HeightMap& TreeMap, float thresholdTreeValue)
{
    int count = 0;
    while (count < numberOfIterations)
    {
        const int xRandIndex = rand() % MAP_RESOLUTION;
        const int yRandIndex = rand() % MAP_RESOLUTION;
        if (TreeMap.At(yRandIndex, xRandIndex) >= thresholdTreeValue)
        {
            const glm::vec3 treePosition = terrainData.vertices[yRandIndex * MAP_RESOLUTION + xRandIndex].Position;
            glm::mat4 TreeModelModel = glm::mat4(1.0f);
            TreeModelModel = glm::translate(TreeModelModel, treePosition);
            TreeModelModel = glm::scale(TreeModelModel, glm::vec3(0.03f, 0.017f, 0.03f));
            treeModels.emplace_back(TreeModelModel);
        }
        count++;

    }
}
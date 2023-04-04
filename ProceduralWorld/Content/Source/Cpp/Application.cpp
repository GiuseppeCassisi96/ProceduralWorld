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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void recompute_heightMap(Shader& NoiseShader, Texture& NoiseText, Texture& BiomeNoiseText, TerrainGeneration& terrain);
GLFWwindow* Setup(GLFWwindow* window);

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
float amplitude = 2.0f;
float frequency = 3.0f;
int octaves = 10;
HeightMap ElevationMap(MAP_RESOLUTION, MAP_RESOLUTION);
HeightMap BiomeMap(MAP_RESOLUTION, MAP_RESOLUTION);
unsigned int TerrainSubLocationIndex, ModelSubLocationIndex;
//Light
glm::vec3 lightDir = glm::vec3(-1.0f, 0.3f, 0.0f);
float lightIntensity = 1.5f;
//Model

int main()
{
    
    float oldFrequency = frequency;
    float oldAmplitude = amplitude;
    int oldOctaves = octaves;
    GLFWwindow* window = nullptr;
    window = Setup(window);
    Tree TreeModel((modelsPath + "tree_obj.obj").c_str());
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
    NoiseText.BindTexture(GL_TEXTURE0);

    Texture BiomeNoiseText{ GL_TEXTURE1, MAP_RESOLUTION, MAP_RESOLUTION };
    BiomeNoiseText.BindTexture(GL_TEXTURE1);
    NoiseShader.DispatchCompute();
    BiomeNoiseText.GetValuesFromTexture(BiomeMap.GetData());
    BiomeNoiseText.BindTexture(GL_TEXTURE1);

    Texture GrassTextures((texturesPath + "Grass.jpg").c_str(), GL_TEXTURE2);
    Texture DesertTextures((texturesPath + "Sand.jpg").c_str(), GL_TEXTURE3);
    Texture SnowTextures((texturesPath + "Snow.jpg").c_str(), GL_TEXTURE4);
    Texture RockTextures((texturesPath + "Rock.jpg").c_str(), GL_TEXTURE5);
    
    GrassTextures.BindTexture(GL_TEXTURE2);
    DesertTextures.BindTexture(GL_TEXTURE3);
    SnowTextures.BindTexture(GL_TEXTURE4);
    RockTextures.BindTexture(GL_TEXTURE5);

    Shader terrainShader{ (shadersPath + "TerrainShader.vert").c_str(),
                      (shadersPath + "TerrainShader.frag").c_str() };

    TerrainGeneration terrain (ElevationMap, BiomeMap);
	glm::mat4 TerrainModel = glm::mat4(1.0f);
    glm::mat3 TerrainNormalMatrix = glm::mat3(1.0f);

    glm::mat4 TreeModelModel = glm::mat4(1.0f);
    glm::mat3 TreeNormalMatrix = glm::mat3(1.0f);

    TerrainModel = glm::translate(TerrainModel, glm::vec3(0.0f));
    TerrainNormalMatrix = glm::inverseTranspose(glm::mat3(TerrainModel));

    TreeModelModel = glm::translate(TreeModelModel, glm::vec3(-300.0f, 0.0f, 0.0f));
    TreeNormalMatrix = glm::inverseTranspose(glm::mat3(TreeModelModel));

    TerrainSubLocationIndex = glGetSubroutineIndex(terrainShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForTerrain");
    ModelSubLocationIndex = glGetSubroutineIndex(terrainShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForModels");

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubLocationIndex);

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
    terrainShader.SetUniformInt("ElevationMap", 0);
    terrainShader.SetUniformInt("BiomeMap", 1);
    terrainShader.SetUniformInt("Grass", terrain.terrainMaterial.Grass);
    terrainShader.SetUniformInt("Desert", terrain.terrainMaterial.Sand);
    terrainShader.SetUniformInt("Snow", terrain.terrainMaterial.Snow);
    terrainShader.SetUniformInt("Rock", terrain.terrainMaterial.Rock);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubLocationIndex);
    terrainShader.SetUniformMatrix4("model", TreeModelModel);
    terrainShader.SetUniformVec3("specularColor", TreeModel.treeMaterial.specularColor);
    terrainShader.SetUniformVec3("albedo", TreeModel.treeMaterial.albedo);
    terrainShader.SetUniformFloat("Ka", TreeModel.treeMaterial.Ka);
    terrainShader.SetUniformFloat("Ks", TreeModel.treeMaterial.Ks);
    terrainShader.SetUniformFloat("Kd", TreeModel.treeMaterial.Kd);
    terrainShader.SetUniformFloat("shininess", TreeModel.treeMaterial.shininess);
    terrainShader.SetUniformFloat("lightIntensity", lightIntensity);
    
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
        terrainShader.UseProgram();
        terrainShader.SetUniformMatrix4("view", WorldCamera);
        terrainShader.SetUniformMatrix4("proj", WorldProjection);
        terrainShader.SetUniformVec3("cameraPos", playerMovement.position);
        terrainShader.SetUniformVec3("lightDir", lightDir);
        terrainShader.SetUniformFloat("lightIntensity", lightIntensity);
        if (isWireframe)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubLocationIndex);
        terrainShader.SetUniformMatrix3("normalMatrix", TerrainNormalMatrix);
        terrainShader.SetUniformMatrix4("model", TerrainModel);
        terrain.DrawTerrain();

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubLocationIndex);
        terrainShader.SetUniformMatrix3("normalMatrix", TreeNormalMatrix);
        terrainShader.SetUniformMatrix4("model", TreeModelModel);
        TreeModel.DrawTree();

        //Setting of whole application UI
        ImGui::Begin("ProceduralWorld control panel ");
        ImGui::Text("Here you can modify all terrain generation parameters");
        ImGui::SliderFloat("Frequency", &frequency, 0.1f, 3.5f);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.1f, 3.0f);
        ImGui::SliderInt("Octaves", &octaves, 2, 10);
        ImGui::SliderFloat("XLightDir", &lightDir.x, -1.0f, 1.0f);
        ImGui::SliderFloat("YLightDir", &lightDir.y, 0.0f, 1.0f);
        ImGui::SliderFloat("LightIntensity", &lightIntensity, 0.0f, 7.0f);
        ImGui::End();

        //Rendering of application UI
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(oldFrequency != frequency || oldAmplitude != amplitude || oldOctaves != octaves)
        {
            recompute_heightMap(NoiseShader, NoiseText, BiomeNoiseText,terrain);
            oldFrequency = frequency;
            oldAmplitude = amplitude;
            oldOctaves = octaves;
            terrain.ReComputeMesh();
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
void recompute_heightMap(Shader& NoiseShader, Texture& NoiseText, Texture& BiomeNoiseText, TerrainGeneration& terrain)
{
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);
    
    NoiseText.BindTexture(GL_TEXTURE0);
    NoiseShader.DispatchCompute();
    BiomeNoiseText.BindTexture(GL_TEXTURE1);
    NoiseShader.DispatchCompute();
    
    NoiseText.GetValuesFromTexture(terrain.ElevationMap.GetData());
    NoiseText.BindTexture(GL_TEXTURE0);
    BiomeNoiseText.GetValuesFromTexture(terrain.BiomeMap.GetData());
    BiomeNoiseText.BindTexture(GL_TEXTURE1);
}

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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void recompute_heightMap(float frequency, Shader& NoiseShader, Texture& NoiseText, TerrainGeneration& terrain);
GLFWwindow* Setup(GLFWwindow* window);

//Global vars
bool isWireframe = false;
bool isMouseVisible = false;
glm::mat4 WorldCamera = glm::mat4(1.0f);
glm::mat4 WorldProjection = glm::perspective(45.0f, static_cast<float>(WIDTH) / static_cast<float>(HEIGHT),
    0.1f, 1000000.0f);
Movement playerMovement{ glm::vec3(0.0f, 0.0f, 7.0f), WorldCamera };
std::string shadersPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Cpp/Shaders/";
float amplitude = 2.0f;
float frequency = 3.0f;
int octaves = 10;
float seed = 1.0f;
HeightMap ElevationMap(MAP_RESOLUTION, MAP_RESOLUTION);
//Light
glm::vec3 lightDir = glm::vec3(1.0f, 0.3f, 0.0f);
float lightIntensity = 1.5f;

int main()
{
    float oldFrequency = frequency;
    float oldAmplitude = amplitude;
    float oldSeed = seed;
    int oldOctaves = octaves;
    GLFWwindow* window = nullptr;
    window = Setup(window);

    //Creation of HeightMap
    Shader NoiseShader{ (shadersPath + "NoiseGeneration.comp").c_str() };
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);
    NoiseShader.SetUniformFloat("seed", seed);
    Texture NoiseText{ GL_TEXTURE0, MAP_RESOLUTION, MAP_RESOLUTION };
    NoiseText.BindTexture(GL_TEXTURE0);
    NoiseShader.DispatchCompute();
    NoiseText.GetValuesFromTexture(ElevationMap.GetData());
    NoiseText.BindTexture(GL_TEXTURE0);

    Shader terrainShader{ (shadersPath + "TerrainShader.vert").c_str(),
                      (shadersPath + "TerrainShader.frag").c_str() };

    TerrainGeneration terrain (ElevationMap);
	glm::mat4 TerrainModel = glm::mat4(1.0f);
    glm::mat3 TerrainNormalMatrix = glm::mat3(1.0f);

    TerrainModel = glm::translate(TerrainModel, glm::vec3(0.0f));
    TerrainNormalMatrix = glm::inverseTranspose(glm::mat3(TerrainModel));
    terrainShader.UseProgram();
    terrainShader.SetUniformMatrix4("model", TerrainModel);
    terrainShader.SetUniformMatrix4("view", WorldCamera);
    terrainShader.SetUniformMatrix4("proj", WorldProjection);
    terrainShader.SetUniformMatrix3("normalMatrix", TerrainNormalMatrix);
    terrainShader.SetUniformVec3("viewPos", playerMovement.position);
    terrainShader.SetUniformVec3("ambientColor", terrain.terrainMaterial.ambientColor);
    terrainShader.SetUniformVec3("diffuseColor", terrain.terrainMaterial.diffusiveColor);
    terrainShader.SetUniformVec3("specularColor", terrain.terrainMaterial.specularColor);
    terrainShader.SetUniformVec3("lightDir", lightDir);
    terrainShader.SetUniformFloat("Ka", terrain.terrainMaterial.Ka);
    terrainShader.SetUniformFloat("Ks", terrain.terrainMaterial.Ks);
    terrainShader.SetUniformFloat("Kd", terrain.terrainMaterial.Kd);
    terrainShader.SetUniformFloat("shininess", terrain.terrainMaterial.shininess);
    terrainShader.SetUniformFloat("lightIntensity", lightIntensity);

    
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);
        playerMovement.Move(window, WorldCamera);

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
        
        terrain.DrawTerrain();

        ImGui::Begin("ProceduralWorld control panel ");
        ImGui::Text("Here you can modify all terrain generation parameters");
        ImGui::SliderFloat("Frequency", &frequency, 0.1f, 3.0f);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.1f, 2.0f);
        ImGui::SliderFloat("Seed", &seed, 0.1f, 3.0f);
        ImGui::SliderInt("Octaves", &octaves, 2, 10);
        ImGui::SliderFloat("XLightDir", &lightDir.x, -1.0f, 1.0f);
        ImGui::SliderFloat("YLightDir", &lightDir.y, 0.0f, 1.0f);
        ImGui::SliderFloat("LightIntensity", &lightIntensity, 0.0f, 7.0f);
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if(oldFrequency != frequency || oldAmplitude != amplitude || oldOctaves != octaves || oldSeed != seed)
        {
            recompute_heightMap(frequency, NoiseShader, NoiseText, terrain);
            oldFrequency = frequency;
            oldAmplitude = amplitude;
            oldOctaves = octaves;
            oldSeed = seed;
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
    ImGui::StyleColorsLight();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
    return window;
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!isMouseVisible)
		playerMovement.Rotate(xpos, ypos);
}
void recompute_heightMap(float frequency, Shader& NoiseShader, Texture& NoiseText, TerrainGeneration& terrain)
{
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);
    NoiseShader.SetUniformFloat("seed", seed);
    NoiseText.BindTexture(GL_TEXTURE0);
    NoiseShader.DispatchCompute();
    NoiseText.GetValuesFromTexture(terrain.ElevationMap.GetData());
    NoiseText.BindTexture(GL_TEXTURE0);
}

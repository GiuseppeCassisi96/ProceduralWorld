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
#include "FrameBuffer.h"

//Callback function
/**
 * \brief Will be called when the user resize the viewport. When she is called, resize the opengl
 * viewport using glViewport() function
 * \param window The reference to window created in the setup phase
 * \param width  The new width of screen 
 * \param height The new height of screen
 */
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
//Will be called when the user press a key of the keyboard
void CommandsInputCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
//Will be called when the user move the mouse
void MouseInputCallback(GLFWwindow* window, double xpos, double ypos);
/**
 * \brief It recomputes the heightmaps using a compute shader. It overwrites the textures
 * precedently written with new height values
 * \param NoiseShader The shader used by the compute shader
 * \param ElevationMapTex Texture of ElevationMap
 * \param BiomeMapTex Texture of BiomeMap
 * \param TreeMapTex Texture of TreeMap
 * \param TerrainData The data of world terrain
 */
void RecomputeHeightMap(Shader& NoiseShader, Texture& ElevationMapTex, Texture& BiomeMapTex, Texture& TreeMapTex, TerrainGeneration& TerrainData);
//Initial setup
GLFWwindow* Setup(GLFWwindow* window);
/**
 * \brief Setups the trees positions. The position of each tree is based on terrain data.
 * To decide if a tree must be placed or not I use the TreeMap and threshold tree value
 * \param terrainData The data of world terrain
 * \param numberOfIterations Number of iterations needed to setup tree positions
 * \param NoiseTreeMap An heightmap used to decide if the tree must be placed or not.
 * She Is compared with the 'thresholdTreeValue'
 * \param thresholdTreeV A float value used to decide if the tree must be placed or not.
 * It is compared with heights value of the TreeMap
 */
void SetupTreePositions(TerrainGeneration& terrainData, int numberOfIterations, HeightMap& NoiseTreeMap, float thresholdTreeV);

//HeightMaps
HeightMap ElevationMap(MAP_RESOLUTION, MAP_RESOLUTION);
HeightMap BiomeMap(MAP_RESOLUTION, MAP_RESOLUTION);
HeightMap TreeMap(MAP_RESOLUTION, MAP_RESOLUTION);

//Global vars
/***\brief Is a bool var that tell if the mouse is visible or not */
bool isMouseVisible = false;
/***\brief Is the view matrix, will be updated when the player moves into the space and rotates*/
glm::mat4 WorldCamera = glm::mat4(1.0f);
/*** \brief Is the proj matrix*/
glm::mat4 WorldProjection = glm::perspective(45.0f, WIDTH / HEIGHT,
                                             0.1f, 1000000.0f);
/*** \brief Is responsible for player movement and rotation*/
Movement playerMovement{ glm::vec3(0.0f, 0.0f, 7.0f), WorldCamera };

//ASSETS PATH:
std::string shadersPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Cpp/Shaders/";
std::string texturesPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Textures/";
std::string modelsPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Models/";
std::string skyPath = "C:/UNIMI/ProceduralWorldProgetto/ProceduralWorld/ProceduralWorld/Content/Source/Textures/SkyBox/";

//TERRAIN GENERATION VAR:
float amplitude = 3.0f;
float frequency = 1.6f;
int octaves = 10;
float thresholdTreeValue = 0.5f;
std::vector<glm::mat4> treeModels;

//SHADERS SUBROUTINE LOCATION:
unsigned int TerrainSubFragmentLoc, ModelSubFragmentLoc, SkySubFragmentLoc;
unsigned int TerrainSubVertexLoc, ModelSubVertexLoc, SkySubVertexLoc;
unsigned int NOEffectLoc, OutlineEffectLoc, GrayScaleEffectLoc, PixelEffectLoc, NightVisionEffectLoc;
unsigned int CurrentEffectLoc;

//EFFECT VARS:
float pixel = 512.0f;
bool toonShadingIsEnabled = false;

//LIGHT VARS:
glm::vec3 lightDir = glm::vec3(0.6f, 0.3f, 0.0f);
float lightIntensity = 2.5f;




int main()
{
    float oldFrequency = frequency;
    float oldAmplitude = amplitude;
    int oldOctaves = octaves;
    float oldThresholdTreeValue = thresholdTreeValue;
    float oldTerrainKs;
    float oldTreeKs;
    GLFWwindow* window = nullptr;
    window = Setup(window);

    Shader NoiseShader{ (shadersPath + "NoiseGeneration.comp").c_str() };
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);
    NoiseShader.SetUniformFloat("seed", 0.0f);

    //Creation of HeightMaps:
    //Here I create an empty heightmap texture 
    Texture ElevationMapText{ GL_TEXTURE0, MAP_RESOLUTION, MAP_RESOLUTION };
    ElevationMapText.BindTexture(GL_TEXTURE0);
    /*I execute the compute shader in order to compute the noise function and write the noise value
     * into a texture. I use a compute shader in order to speed up this processes*/
    NoiseShader.DispatchCompute();
    //I write the texture data into my HeightMap 
    ElevationMapText.GetValuesFromTexture(ElevationMap.GetData());
    //I will do the same things for all other heightmap

    Texture BiomeMapText{ GL_TEXTURE1, MAP_RESOLUTION, MAP_RESOLUTION };
    BiomeMapText.BindTexture(GL_TEXTURE1);
    NoiseShader.DispatchCompute();
    BiomeMapText.GetValuesFromTexture(BiomeMap.GetData());

    Texture TreeMapText{ GL_TEXTURE2, MAP_RESOLUTION, MAP_RESOLUTION };
    TreeMapText.BindTexture(GL_TEXTURE2);
    NoiseShader.DispatchCompute();
    TreeMapText.GetValuesFromTexture(TreeMap.GetData());

    //I create the three color texture of my game world
    Texture StylizedGrassTexture((texturesPath + "StylizedGrass.jpg").c_str(), GL_TEXTURE3);
    Texture StylizedLeavesTexture((texturesPath + "StylizedLeaves.jpg").c_str(), GL_TEXTURE4);
    Texture StylizedRockTexture((texturesPath + "StylizedRock.jpg").c_str(), GL_TEXTURE5);
    //I bind each texture to her texture unit
    StylizedGrassTexture.BindTexture(GL_TEXTURE3);
    StylizedLeavesTexture.BindTexture(GL_TEXTURE4);
    StylizedRockTexture.BindTexture(GL_TEXTURE5);

    /*The worldShader is the shader for the whole world(terrain,tree,skybox). I use subroutines to handle
     *each specific case*/
    Shader worldShader{ (shadersPath + "WorldShader.vert").c_str(),
                      (shadersPath + "WorldShader.frag").c_str() };
    /*The postProcessEffectShader is the shader for post processing effects. I use subroutines to handle
     *each specific effect*/
    Shader postProcessEffectShader{ (shadersPath + "FB.vert").c_str(),
                      (shadersPath + "FB.frag").c_str() };
    /*I create the world terrain where the height of each vertex is based on the height value of
     *the ElevationMap*/
    TerrainGeneration terrain (ElevationMap);

	glm::mat4 TerrainModel = glm::mat4(1.0f);
    glm::mat3 TerrainNormalMatrix = glm::mat3(1.0f);

    TerrainModel = glm::translate(TerrainModel, glm::vec3(0.0f));
    TerrainNormalMatrix = glm::inverseTranspose(glm::mat3(TerrainModel));

    //I obtain the locations of the subroutines in the fragment shader 
    TerrainSubFragmentLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForTerrain");
    ModelSubFragmentLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_FRAGMENT_SHADER,
        "illuminationForModels");
	SkySubFragmentLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_FRAGMENT_SHADER,
        "SkyBoxFrag");

    //I obtain the locations of the subroutines in the vertex shader 
    TerrainSubVertexLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_VERTEX_SHADER,
        "TerrainVert");
    ModelSubVertexLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_VERTEX_SHADER,
        "TreeVert");
    SkySubVertexLoc = glGetSubroutineIndex(worldShader.GetProgram(), GL_VERTEX_SHADER,
        "SkyBoxVert");

    /*I obtain the locations of the subroutines in the fragment shader.Each subroutine represents
     *a different post-processing effect*/
    NOEffectLoc = glGetSubroutineIndex(postProcessEffectShader.GetProgram(), GL_FRAGMENT_SHADER,
        "NOEffect");
    OutlineEffectLoc = glGetSubroutineIndex(postProcessEffectShader.GetProgram(), GL_FRAGMENT_SHADER,
        "OutlineEffect");
    GrayScaleEffectLoc = glGetSubroutineIndex(postProcessEffectShader.GetProgram(), GL_FRAGMENT_SHADER,
        "GrayScaleEffect");
    PixelEffectLoc = glGetSubroutineIndex(postProcessEffectShader.GetProgram(), GL_FRAGMENT_SHADER,
        "PixelEffect");
    NightVisionEffectLoc = glGetSubroutineIndex(postProcessEffectShader.GetProgram(), GL_FRAGMENT_SHADER,
        "NightVisionEffect");

    //I create the Framebuffer
    FrameBuffer frame_buffer(GL_TEXTURE7);
    postProcessEffectShader.UseProgram();
    //I set the sampler2D with the correspond texture unit. Then I set some uniforms
    postProcessEffectShader.SetUniformInt("frameTexture", 7);
    postProcessEffectShader.SetUniformFloat("screenWidth", WIDTH);
    postProcessEffectShader.SetUniformFloat("screenHeight", HEIGHT);
    postProcessEffectShader.SetUniformFloat("pixel", pixel);
    CurrentEffectLoc = NOEffectLoc;

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubFragmentLoc);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &TerrainSubFragmentLoc);

    worldShader.UseProgram();
    worldShader.SetUniformMatrix4("model", TerrainModel);
    worldShader.SetUniformMatrix4("view", WorldCamera);
    worldShader.SetUniformMatrix4("proj", WorldProjection);
    worldShader.SetUniformVec3("viewPos", playerMovement.position);
    worldShader.SetUniformVec3("specularColor", terrain.terrainMaterial.specularColor);
    worldShader.SetUniformVec3("lightDir", lightDir);
    worldShader.SetUniformFloat("Ka", terrain.terrainMaterial.Ka);
    worldShader.SetUniformFloat("Ks", terrain.terrainMaterial.Ks);
    worldShader.SetUniformFloat("Kd", terrain.terrainMaterial.Kd);
    worldShader.SetUniformFloat("shininess", terrain.terrainMaterial.shininess);
    worldShader.SetUniformFloat("lightIntensity", lightIntensity);
    //I set each sampler2D with the his correspond texture unit
    worldShader.SetUniformInt("BiomeMap", 1);
    worldShader.SetUniformInt("Lawn", terrain.terrainMaterial.Lawn);
    worldShader.SetUniformInt("Forest", terrain.terrainMaterial.Forest);
    worldShader.SetUniformInt("Mountain", terrain.terrainMaterial.Mountain);
    worldShader.SetUniformBool("toonShadingIsEnabled", false);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubFragmentLoc);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &ModelSubVertexLoc);

    SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap, thresholdTreeValue);
    //Load the tree model
    Tree TreeModel(modelsPath + "Tree.fbx",treeModels);
    worldShader.SetUniformVec3("specularColor", TreeModel.treeMaterial.specularColor);
    worldShader.SetUniformFloat("Ka", TreeModel.treeMaterial.Ka);
    worldShader.SetUniformFloat("Ks", TreeModel.treeMaterial.Ks);
    worldShader.SetUniformFloat("Kd", TreeModel.treeMaterial.Kd);
    worldShader.SetUniformFloat("shininess", TreeModel.treeMaterial.shininess);
    worldShader.SetUniformFloat("lightIntensity", lightIntensity);

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &SkySubFragmentLoc);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &SkySubVertexLoc);
    //I Create the skybox and set his sampler2D uniform 
    SkyBox sky(skyPath);
    worldShader.SetUniformInt("skybox", 6);

    oldTerrainKs = terrain.terrainMaterial.Ks;
    oldTreeKs = TreeModel.treeMaterial.Ks;

    //RENDER LOOP
    while (!glfwWindowShouldClose(window))
    {
        //INPUT:
        playerMovement.Move(window, WorldCamera);

        //ImGuI FRAME CREATION:
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //RENDER:
        frame_buffer.BindFrameBuffer();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        //Terrain rendering
        worldShader.UseProgram();
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &TerrainSubFragmentLoc);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &TerrainSubVertexLoc);
        worldShader.SetUniformMatrix3("normalMatrix", TerrainNormalMatrix);
        worldShader.SetUniformMatrix4("model", TerrainModel);
        worldShader.SetUniformMatrix4("view", WorldCamera);
        worldShader.SetUniformVec3("cameraPos", playerMovement.position);
        worldShader.SetUniformVec3("lightDir", lightDir);
        worldShader.SetUniformFloat("lightIntensity", lightIntensity);
        worldShader.SetUniformFloat("Ka", terrain.terrainMaterial.Ka);
        worldShader.SetUniformFloat("Ks", terrain.terrainMaterial.Ks);
        worldShader.SetUniformFloat("Kd", terrain.terrainMaterial.Kd);
        worldShader.SetUniformFloat("shininess", terrain.terrainMaterial.shininess);
        worldShader.SetUniformVec3("specularColor", terrain.terrainMaterial.specularColor);
        worldShader.SetUniformBool("toonShadingIsEnabled", toonShadingIsEnabled);
        terrain.DrawTerrain();

        //Tree rendering
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &ModelSubFragmentLoc);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &ModelSubVertexLoc);
        worldShader.SetUniformVec3("specularColor", TreeModel.treeMaterial.specularColor);
        worldShader.SetUniformFloat("Ka", TreeModel.treeMaterial.Ka);
        worldShader.SetUniformFloat("Ks", TreeModel.treeMaterial.Ks);
        worldShader.SetUniformFloat("Kd", TreeModel.treeMaterial.Kd);
        worldShader.SetUniformFloat("shininess", TreeModel.treeMaterial.shininess);
        TreeModel.DrawTree();

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &SkySubFragmentLoc);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, 1, &SkySubVertexLoc);

        //change depth function so depth test passes when the depth value are equal to depth buffer's content
        glDepthFunc(GL_LEQUAL);
        glm::mat4 skyView = glm::mat4(1.0f);
        skyView = glm::mat4(glm::mat3(WorldCamera));
        worldShader.SetUniformMatrix4("view", skyView);
        sky.DrawSkyBox(GL_TEXTURE6);
        //set depth function back to default
        glDepthFunc(GL_LESS);

        frame_buffer.UNBindFrameBuffer();
        postProcessEffectShader.UseProgram();
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &CurrentEffectLoc);
        postProcessEffectShader.SetUniformFloat("pixel", pixel);
        frame_buffer.DrawFrameBuffer();

        //SETTING APP UI:
        ImGui::Begin("ProceduralWorld control panel ");
        ImGui::Text("Here you can modify all terrain generation parameters");
        ImGui::SliderFloat("Frequency", &frequency, 0.1f, 3.5f);
        ImGui::SliderFloat("Amplitude", &amplitude, 0.1f, 3.0f);
        ImGui::SliderInt("Octaves", &octaves, 2, 10);
        ImGui::SliderFloat("XLightDir", &lightDir.x, -1.0f, 1.0f);
        ImGui::SliderFloat("YLightDir", &lightDir.y, 0.0f, 1.0f);
        ImGui::SliderFloat("LightIntensity", &lightIntensity, 0.0f, 7.0f);
        ImGui::SliderFloat("Threshold tree value", &thresholdTreeValue, -3.0f, 3.0f);
        ImGui::Checkbox("ToonShading", &toonShadingIsEnabled);
        if(ImGui::Button("NoPostProcessEffect"))
        {
            CurrentEffectLoc = NOEffectLoc;
            terrain.terrainMaterial.Ks = oldTerrainKs;
            TreeModel.treeMaterial.Ks = oldTreeKs;
        }
        if (ImGui::Button("OutlineEffect"))
        {
            CurrentEffectLoc = OutlineEffectLoc;
            terrain.terrainMaterial.Ks = 0.0f;
            TreeModel.treeMaterial.Ks = 0.0f;
        }
        if (ImGui::Button("GrayScaleEffect"))
        {
            CurrentEffectLoc = GrayScaleEffectLoc;
            terrain.terrainMaterial.Ks = 0.0f;
            TreeModel.treeMaterial.Ks = 0.0f;
        }
        if (ImGui::Button("PixelEffect"))
        {
            CurrentEffectLoc = PixelEffectLoc;
            terrain.terrainMaterial.Ks = 0.0f;
            TreeModel.treeMaterial.Ks = 0.0f;
        }
        ImGui::SliderFloat("Pixels Value", &pixel, 512, 2048);
        if (ImGui::Button("NightVisionEffect"))
        {
            CurrentEffectLoc = NightVisionEffectLoc;
            terrain.terrainMaterial.Ks = 0.0f;
            TreeModel.treeMaterial.Ks = 0.0f;
        }
        
        ImGui::Text("Frame Time: %fms", static_cast<double>(playerMovement.deltaTime));
        ImGui::End();

        //RENDERING OF APPLICATION UI:
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //UPDATE VALUES:
        if(oldThresholdTreeValue != thresholdTreeValue)
        {
            //If changes only the 'thresholdTreeValue', I only update the tree positions
            oldThresholdTreeValue = thresholdTreeValue;
            //I empty every time the 'treeModels' vector 
            treeModels.clear();
            //Update tree positions
            SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap, thresholdTreeValue);
            /*I update the mesh buffer that contains the model matrix of each tree(I use the instancing
             *technique to draw the trees)*/
            TreeModel.RecomputeTree(treeModels);
        }
        if(oldFrequency != frequency || oldAmplitude != amplitude || oldOctaves != octaves)
        {
            /*Here I recompute also the terrain, because I have changed params for height map
             *generations.*/
            RecomputeHeightMap(NoiseShader,ElevationMapText, BiomeMapText, TreeMapText,terrain);
            //I set the olds values with new values
            oldFrequency = frequency;
            oldAmplitude = amplitude;
            oldOctaves = octaves;
            //Now I recompute the mesh data and update the mesh buffer
            terrain.ReComputeMesh();

            //Recompute Tree positions
            treeModels.clear();
            SetupTreePositions(terrain, TREE_ITERATION_NUMBER, TreeMap,thresholdTreeValue);
            TreeModel.RecomputeTree(treeModels);
        }

        //Swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    terrain.DeleteBuffers();
    frame_buffer.DeleteBuffers();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //Terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void CommandsInputCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    //If the player press the 'ESC' button the application will be closed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    /*If the player press the 'L' button sets the visibility of mouse. If the mouse was visible it will
     *become visible and vice versa. When the mouse is visible the rotation is disabled 
     */
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        isMouseVisible = !isMouseVisible;
        if(isMouseVisible)
        {
            //ENTER IN THE UI CONTROL MODE
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
        {
            //ENTER IN THE FREE MODE
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        
    }
}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
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
    window = glfwCreateWindow(static_cast<int>(WIDTH), static_cast<int>(HEIGHT) ,"ProceduralWorld", glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, MouseInputCallback);
    glfwSetKeyCallback(window, CommandsInputCallback);
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
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
void MouseInputCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(!isMouseVisible)
		playerMovement.Rotate(xpos, ypos);
}
void RecomputeHeightMap(Shader& NoiseShader, Texture& ElevationMapTex, Texture& BiomeMapTex, Texture& TreeMapTex, TerrainGeneration& TerrainData)
{
    NoiseShader.UseProgram();
    NoiseShader.SetUniformFloat("amplitude", amplitude);
    NoiseShader.SetUniformFloat("frequency", frequency);
    NoiseShader.SetUniformInt("octaves", octaves);


    ElevationMapTex.BindTexture(GL_TEXTURE0);
    //I Bind the texture with the image2D of compute shader
    glBindImageTexture(0, ElevationMapTex.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    /*I execute the compute shader in order to compute the noise function and write the noise value
    * into a texture. I use a compute shader in order to speed up this processes*/
    NoiseShader.DispatchCompute();
    //I write the texture data into my HeightMap 
    ElevationMapTex.GetValuesFromTexture(TerrainData.ElevationMap.GetData());
    //I will do the same things for all other heightmap

    BiomeMapTex.BindTexture(GL_TEXTURE1);
    glBindImageTexture(0, BiomeMapTex.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    NoiseShader.DispatchCompute();
    BiomeMapTex.GetValuesFromTexture(BiomeMap.GetData());

    TreeMapTex.BindTexture(GL_TEXTURE2);
    glBindImageTexture(0, TreeMapTex.textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
    NoiseShader.DispatchCompute();
    TreeMapTex.GetValuesFromTexture(TreeMap.GetData());
}
void SetupTreePositions(TerrainGeneration& terrainData, int numberOfIterations, HeightMap& NoiseTreeMap, float thresholdTreeV)
{
    int count = 0;
    while (count < numberOfIterations)
    {
        //I pick a random column and random row of my TreeMap
        const int randCol = rand() % MAP_RESOLUTION;
        const int randRow = rand() % MAP_RESOLUTION;
        /*If the noise value is greater or equal of 'thresholdTreeV' I create a new
         *model matrix with a new position and a new scale*/
        if (NoiseTreeMap.At(randRow, randCol) >= thresholdTreeV)
        {
            //I pick the position using the random indices computed before
            const glm::vec3 treePosition = terrainData.vertices[randRow * MAP_RESOLUTION + randCol].Position;
            glm::mat4 TreeModelModel = glm::mat4(1.0f);
            //I translate my model using 'treePosition'
            TreeModelModel = glm::translate(TreeModelModel, treePosition);
            TreeModelModel = glm::scale(TreeModelModel, glm::vec3(0.03f, 0.017f, 0.03f));
            /*I add the model matrix in the 'treeModels' vector.I will pass this vector
             *in the tree mesh buffer*/
            treeModels.emplace_back(TreeModelModel);
        }
        count++;

    }
}
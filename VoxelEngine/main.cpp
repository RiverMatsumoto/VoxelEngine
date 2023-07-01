#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp>

#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"
#include "ShaderData.h"
#include "Model.h"

#include <filesystem>
#include <iostream>
#include <vector>
#include <chrono>
#include "Chunk.h"
#include "ChunkManager.h"
#include <PerlinNoise.hpp>

// big fat annoying blocks of code that I don't care about
void setup_window(GLFWwindow *&window, unsigned int width, unsigned int height);
void initImGuiData();
void SetupImGui(GLFWwindow *window);
void SetupImGuiRenderLoop();
void ImGuiRenderLoop();
void updateGuiData();
void processInput(GLFWwindow *window);
void generateCube(unsigned int VBO[], unsigned int VAO[], unsigned int EBO[]);
void registerTextures(unsigned int texture[], const char *paths[], int nTextures);
void setShaderData(Shader& shader);

// Callbacks
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods);

// util
void updateDeltaTime();
void SelectBlock(glm::mat4 projection, glm::mat4 view);

//Mesh createMesh();

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

unsigned int screenWidth = 1600;
unsigned int screenHeight = 1200;
glm::vec3 selectedBlock(0.0f);

float deltaTime = (float) glfwGetTime();
float lastFrame = (float) glfwGetTime();
float currentFrame;
float fps;
float updateFpsDelay = 0.1f;
ShaderData shaderData;
Shader *shaderPtr;

glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
int debugBlockPos[3];

bool useWireframe = false;

unsigned int textures[3];
const char *paths[3] = {
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//grass_minecraft.png",
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//container2_specular.png",
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//brickwall.jpg",
};

struct CameraData
{
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 worldUp;
    float zoom;
};

CameraData camData;
bool useDebugCam = false;
ChunkManager chunkManager;

int main()
{
    GLFWwindow *window{};
    setup_window(window, screenWidth, screenHeight);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_btn_callback);


    const char *vertexPath = "C:\\Users\\river\\source\\repos\\VoxelEngine\\VoxelEngine\\vertexShader.glsl";
    const char *fragmentPath = "C:\\Users\\river\\source\\repos\\VoxelEngine\\VoxelEngine\\fragmentShader.glsl";
    Shader shader = Shader(vertexPath, fragmentPath);
    shaderPtr = &shader;


    const char *lightCubeFragPath = "C://Users//river//CLionProjects//VoxelEngine//src//shaders//lightCubeFrag.glsl";
    Shader lightSourceShader = Shader(vertexPath, lightCubeFragPath);
    // shaders
    lightSourceShader.use();
    lightSourceShader.setVec3("lightColor", glm::vec3(1.0f));
    shaderData.dirLightDirection = glm::vec3(-0.1f, -0.2f, 0.0f);

    // importing models
    //const char* modelVertexPath = "C:/Users/river/source/repos/VoxelEngine/VoxelEngine/modelVertex.glsl";
    //const char *basicFragPath = "C://Users//river//CLionProjects//VoxelEngine//src//shaders//basicFrag.glsl";
    //const char *backpackPath = "C:/Users/river/source/repos/VoxelEngine/VoxelEngine/resources/objects/backpack/backpack.obj";
    //Shader basicShader = Shader(modelVertexPath, basicFragPath);
    //Model backpack(backpackPath);

    registerTextures(textures, paths, 3);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, textures[0]);
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_2D, textures[1]);
    //glActiveTexture(GL_TEXTURE2);
    //glBindTexture(GL_TEXTURE_2D, textures[2]);

    glm::vec3 cubePos(0.0f);

    unsigned int VBOs[10], VAOs[10], EBOs[10];
    generateCube(VBOs, VAOs, EBOs);



    // positions all containers
    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,  0.0f,  0.0f),
            glm::vec3( 2.0f,  5.0f, -15.0f),
            glm::vec3(-1.5f, -2.2f, -2.5f),
            glm::vec3(-3.8f, -2.0f, -12.3f),
            glm::vec3( 2.4f, -0.4f, -3.5f),
            glm::vec3(-1.7f,  3.0f, -7.5f),
            glm::vec3( 1.3f, -2.0f, -2.5f),
            glm::vec3( 1.5f,  2.0f, -2.5f),
            glm::vec3( 1.5f,  0.2f, -1.5f),
            glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    //Mesh myMesh = createMesh();
    chunkManager.InitializeAllChunks();
    SetupImGui(window);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(4.0f);

	//std::string squirrelPath("C:/Users/river/source/repos/VoxelEngine/VoxelEngine/resources/objects/squirrel/Squirrel_OBJ.obj");
	//Model squirrel(squirrelPath);



    while (!glfwWindowShouldClose(window))
    {
        // input
        glfwPollEvents();
        updateDeltaTime();
        processInput(window);

        // imgui stuff
        SetupImGuiRenderLoop();

        // Clear buffers, set background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) screenWidth / (float) screenHeight,
                                                0.1f, 100.0f);
        // translate camera.position - model
        projection = projection * glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, -1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f);

        
        auto start = std::chrono::high_resolution_clock::now();
        SelectBlock(projection, view);
        auto finish = std::chrono::high_resolution_clock::now();
        std::cout << "For selecting: " << (finish - start) << std::endl;

        // rendering
        shader.use();
        shader.setInt("material.texture_diffuse1", 0);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setMat4("model", model);
        setShaderData(shader);

		glm::mat4 debugView = glm::lookAt(camData.position, camData.position + camData.front, camData.worldUp);
        shader.setMat4("debugView", debugView);

		shader.setVec4("debugColor", glm::vec4(0.8f, 0.2f, 0.2f, 0.5f));
		shader.setVec4("normalCamColor", glm::vec4(0.2f, 0.8f, 0.2f, 0.5f));
        shader.setBool("useDebugCam", useDebugCam);
        chunkManager.Render(shader);


        // 32x32x32 big cube
//        glBindVertexArray(VAOs[0]);
//		for (int x = 0; x < 32; x++)
//		{
//			for (int y = 0; y < 32; y++)
//			{
//				for (int z = 0; z < 32; z++)
//				{
//					model = glm::mat4(1.0f);
//					model = glm::translate(model, glm::vec3(x, y, z));
//					shader.setMat4("model", model);
//					glDrawArrays(GL_TRIANGLES, 0, 36);
//				}
//			}
//		}
        // opengl tutorial layouts
        //glBindVertexArray(VAOs[0]);
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    // calculate the model matrix for each object and pass it to shader before drawing
        //    model = glm::mat4(1.0f);
        //    model = glm::translate(model, cubePositions[i]);
        //    float angle = 20.0f * (float)i;
        //    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        //    shader.setMat4("model", model);
        //    glDrawArrays(GL_TRIANGLES, 0, 36);
        //}

         //light cube
        glBindVertexArray(VAOs[1]);
        lightSourceShader.use();
        lightSourceShader.setVec3("lightColor", lightColor);
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);
		model = glm::mat4(1.0);
		model = glm::translate(model, shaderData.pntLightPositions[0]);
		model = glm::scale(model, glm::vec3(0.2f));
		lightSourceShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

        // more imgui stuff
        ImGuiRenderLoop();

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

struct cube
{
    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    glm::vec3 v3;
    glm::vec3 v4;
    glm::vec3 v5;
    glm::vec3 v6;
    glm::vec3 v7;
};

void printVec3(glm::vec3 v)
{
    printf("(%f, %f, %f)\n", v.x, v.y, v.z);
}

void SelectBlock(glm::mat4 projection, glm::mat4 view)
{
	// raycast
	glm::vec3 raycastPos = camera.Position;
	glm::vec3 ray_nds = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, 1.0f, 1.0);
	glm::vec4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0f);
	glm::vec4 ray_world = glm::vec4(glm::inverse(view) * ray_eye);
	glm::vec3 ray_dir(ray_world.x, ray_world.y, ray_world.z);//ray_world = glm::vec4(ray_world.x, ray_world.y, ray_world.z, 0.0f);
	ray_world = glm::normalize(ray_world);
	glm::vec3 ray_origin = camera.Position;
	float planeDistance = 1.0f;
	glm::vec3 planeOrig(0.0f);
	glm::vec3 planeNormal(0.0f, 1.0f, 0.0f);
	glm::vec2 baryPosition(0.0f,0.0f);
	//bool intersects = glm::intersectRayPlane(ray_origin, ray_dir, planeOrig, planeNormal, intersectionDistance);
	glm::vec3 pos = camera.Position;


    std::vector<cube> blocksToCheck;
    int distance = 4;
    for (int x = -distance; x < distance; x++)
    {
        for (int y = -distance; y < distance; y++)
        {
            for (int z = -distance; z < distance; z++)
            {
                cube block;
                block.v0 = glm::vec3((int)(pos.x + x)       , (int)(pos.y + y    ) , (int)(pos.z + z    ));
                if (chunkManager.IsBlockActive(block.v0.x, block.v0.y, block.v0.z))
                {
					block.v1 = glm::vec3((int)(pos.x + x + 1)   , (int)(pos.y + y    ) , (int)(pos.z + z    ));
					block.v2 = glm::vec3((int)(pos.x + x + 1)   , (int)(pos.y + y + 1) , (int)(pos.z + z    ));
					block.v3 = glm::vec3((int)(pos.x + x)       , (int)(pos.y + y + 1) , (int)(pos.z + z    ));
					block.v4 = glm::vec3((int)(pos.x + x)       , (int)(pos.y + y    ) , (int)(pos.z + z + 1));
					block.v5 = glm::vec3((int)(pos.x + x + 1)   , (int)(pos.y + y    ) , (int)(pos.z + z + 1));
					block.v6 = glm::vec3((int)(pos.x + x + 1)   , (int)(pos.y + y + 1) , (int)(pos.z + z + 1));
					block.v7 = glm::vec3((int)(pos.x + x)       , (int)(pos.y + y + 1) , (int)(pos.z + z + 1));
					blocksToCheck.push_back(block);
                }
            }
        }
    }
    std::vector<cube>::iterator ptr;
    std::vector<float> distances;
    std::vector<glm::ivec3> blockPositions;
    for (ptr = blocksToCheck.begin(); ptr < blocksToCheck.end(); ptr++)
    {
        /*
            indices visual
                forward
              7 --- 6
             /|    /|
    left	3 --- 2 | right
            | 4 --| 5
            |/    |/
            0 --- 1
            back
        */
        // check if any of the faces intersect
        float distanceToTriangle = 10.0f;
        glm::vec2 baryPosition(0.0f);
        if (glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v0, ptr->v1, ptr->v2, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v2, ptr->v3, ptr->v0, baryPosition, distanceToTriangle) || 
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v4, ptr->v5, ptr->v6, baryPosition, distanceToTriangle) || 
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v6, ptr->v7, ptr->v4, baryPosition, distanceToTriangle) || 
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v4, ptr->v0, ptr->v3, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v3, ptr->v7, ptr->v4, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v1, ptr->v5, ptr->v6, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v6, ptr->v2, ptr->v1, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v4, ptr->v5, ptr->v1, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v1, ptr->v0, ptr->v4, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v3, ptr->v2, ptr->v6, baryPosition, distanceToTriangle) ||
            glm::intersectRayTriangle(ray_origin, ray_dir, ptr->v6, ptr->v7, ptr->v3, baryPosition, distanceToTriangle))
        {
            if (distanceToTriangle < distance && distanceToTriangle > 0)
            {
                blockPositions.push_back(ptr->v0);
                distances.push_back(distanceToTriangle);
            }
        }
    }

    float minDistance = INFINITY;
    int minIndex = -1;
    for (int i = 0; i < blockPositions.size(); i++)
    {
        if (chunkManager.IsBlockActive(blockPositions[i].x, blockPositions[i].y, blockPositions[i].z))
        {
			//printf("(%d,%d,%d)\n", blockPositions[i].x, blockPositions[i].y, blockPositions[i].z);
            if (minDistance > distances[i])
            {
                minIndex = i;
                minDistance = distances[i];
				selectedBlock = blockPositions[i];
            }
        }
    }
}

void generateCube(unsigned int VBO[], unsigned int VAO[], unsigned int EBO[])
{
    float cube[] = {
            // vertex position x3 | texture map x2 | normal x3
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenBuffers(1, &VBO[0]);
    glGenVertexArrays(1, &VAO[0]);

    glBindVertexArray(VAO[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) 0);
    glEnableVertexAttribArray(0);

    // texture
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // normals
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) (5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);

    // light cube
    glGenVertexArrays(1, &VAO[1]);
    glBindVertexArray(VAO[1]);

    // position
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void *) 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void registerTextures(unsigned int texture[], const char *paths[], int nTextures)
{
    // ideas: use imgui to edit the texture settings such as: flipping texture, rgba setting, and other render settings
    unsigned int activeTexture = 0;
    glGenTextures(nTextures, texture);
    for (int i = 0; i < nTextures; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::filesystem::path filePath = paths[i];
        if (filePath.extension() == ".jpg")
        {
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char *data = stbi_load(paths[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load image data for index: " << i << std::endl;
            }
            stbi_image_free(data);
        }
        else if (filePath.extension() == ".png")
        {
            std::cout << "Deez nutz" << std::endl;
            glBindTexture(GL_TEXTURE_2D, texture[i]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            int width, height, nrChannels;
            unsigned char *data = stbi_load(paths[i], &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load image data for index: " << i << std::endl;
            }
            stbi_image_free(data);
        }
    }
}


void SetupImGuiRenderLoop()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiRenderLoop()
{
    ImGui::Begin("Gargelon");
    updateFpsDelay -= deltaTime;
    if (updateFpsDelay < 0)
    {
        fps = 1 / deltaTime;
        updateFpsDelay = 0.1f;
    }
    ImGui::Text("fps: %.1f", fps);

    ImGui::SliderFloat3("Light Direction", glm::value_ptr(shaderData.dirLightDirection), -1.0f, 1.0f);
    ImGui::InputFloat("Speed", &camera.MovementSpeed);

    ImGui::Text("Point light 0 position: (%f, %f, %f)",
                shaderData.pntLightPositions[0].x,
                shaderData.pntLightPositions[0].y,
                shaderData.pntLightPositions[0].z);
    ImGui::Text("Position: (%f, %f, %f)",
        camera.Position.x,
        camera.Position.y,
        camera.Position.z);
    if (ImGui::Button("Toggle Wireframe"))
    {
        if (useWireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        useWireframe = !useWireframe;
    }

    ImGui::Text("Debug Cam Position: (%f, %f, %f)", camData.position.x, camData.position.y, camData.position.z);
    ImGui::Text("Debug Cam Front (%f, %f, %f)", camData.front.x, camData.front.y, camData.front.z);
    ImGui::SliderInt3("Block To Remove", debugBlockPos, 0, 32 * 4);
    if (ImGui::Button("Remove Block"))
    {
        chunkManager.RemoveBlock(debugBlockPos[0], debugBlockPos[1], debugBlockPos[2]);
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SetupImGui(GLFWwindow *window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void) io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void processInput(GLFWwindow *window)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard) return;

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        if (camera.movementEnabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            io.WantCaptureMouse = true;
            camera.DisableMovement();
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            io.WantCaptureMouse = false;
            camera.EnableMovement();
        }
    }
    if (key == GLFW_KEY_SLASH && action == GLFW_PRESS)
    {
		camData.position = camera.Position;
		camData.front = camera.Front;
		camData.worldUp = camera.WorldUp;
		camData.zoom = camera.Zoom;
        useDebugCam = !useDebugCam;
    }
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (camera.firstMouse)
    {
        camera.lastX = xpos;
        camera.lastY = ypos;
        camera.firstMouse = false;
    }

    float xoffset = xpos - camera.lastX;
    float yoffset = camera.lastY - ypos;

    camera.lastX = xpos;
    camera.lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float) yoffset);
}

void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseButtonEvent(button, action);

    if (io.WantCaptureMouse) return;

    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
    {
        chunkManager.RemoveBlock(selectedBlock);
    }
}

void updateDeltaTime()
{
    currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, height, width);
}

void setup_window(GLFWwindow *&window, unsigned int screenWidth, unsigned int screenHeight)
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window and error check
    window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    // Setting size of the window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
}

// big boi func
void setShaderData(Shader& shader)
{
    shader.use();
    shader.setVec3("viewPos", camData.position);
    shader.setBool("useDirectionalLight", true);
    shader.setBool("usePointlights", false);
    shader.setBool("useSpotLight", false);

    shader.setVec3("dirLight.direction", shaderData.dirLightDirection);
    shader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("dirLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

    // point light 1
    // light orbiting around 0.0 equation :)
    shaderData.pntLightPositions[0] = glm::vec3(sin(glfwGetTime()) + 0.5f, sin(glfwGetTime() * 1.2f) + 0.5f, cos(glfwGetTime()) + 0.5f);
    shader.setVec3("pointLights[0].position", shaderData.pntLightPositions[0]);
    shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[0].diffuse", 0.7f, 0.7f, 0.7f);
    shader.setVec3("pointLights[0].specular", 0.1f, 0.1f, 0.1f);
    shader.setFloat("pointLights[0].constant", 1.0f);
    shader.setFloat("pointLights[0].linear", 0.09f);
    shader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    shader.setVec3("pointLights[1].position", shaderData.pntLightPositions[1]);
    shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[1].specular", 0.1f, 0.1f, 0.1f);
    shader.setFloat("pointLights[1].constant", 1.0f);
    shader.setFloat("pointLights[1].linear", 0.09f);
    shader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    shader.setVec3("pointLights[2].position", shaderData.pntLightPositions[2]);
    shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[2].specular", 0.1f, 0.1f, 0.1f);
    shader.setFloat("pointLights[2].constant", 1.0f);
    shader.setFloat("pointLights[2].linear", 0.09f);
    shader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    shader.setVec3("pointLights[3].position", shaderData.pntLightPositions[3]);
    shader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    shader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("pointLights[3].specular", 0.1f, 0.1f, 0.1f);
    shader.setFloat("pointLights[3].constant", 1.0f);
    shader.setFloat("pointLights[3].linear", 0.09f);
    shader.setFloat("pointLights[3].quadratic", 0.032f);

    shader.setVec3("spotLight.position", camData.position);
    shader.setVec3("spotLight.direction", camData.front);
    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 0.3f, 0.3f, 0.3f);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));
}

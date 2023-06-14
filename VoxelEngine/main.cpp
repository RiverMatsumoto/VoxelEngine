#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "stb_image.h"
#include "Camera.h"
#include "ShaderData.h"
//#include "Model.h"

#include <filesystem>
#include <iostream>
#include <vector>
#include "Chunk.h"

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

//Mesh createMesh();

Camera camera(glm::vec3(0.0f, 0.0f, 0.0f));

unsigned int screenWidth = 1600;
unsigned int screenHeight = 1200;

float deltaTime = (float) glfwGetTime();
float lastFrame = (float) glfwGetTime();
float currentFrame;
float fps;
float updateFpsDelay = 0.1f;
ShaderData shaderData;
Shader *shaderPtr;

glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

bool useWireframe = false;

unsigned int textures[3];
const char *paths[3] = {
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//container2.png",
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//container2_specular.png"
        "C://Users//river//source//repos//VoxelEngine//VoxelEngine//resources//textures//brickwall.jpg"
};

int main()
{
    GLFWwindow *window{};
    setup_window(window, screenWidth, screenHeight);
    SetupImGui(window);

    const char *vertexPath = "C:\\Users\\river\\source\\repos\\VoxelEngine\\VoxelEngine\\vertexShader.glsl";
    const char *fragmentPath = "C:\\Users\\river\\source\\repos\\VoxelEngine\\VoxelEngine\\fragmentShader.glsl";
    Shader shader = Shader(vertexPath, fragmentPath);
    shaderPtr = &shader;


    const char *lightCubeFragPath = "C://Users//river//CLionProjects//VoxelEngine//src//shaders//lightCubeFrag.glsl";
    Shader lightSourceShader = Shader(vertexPath, lightCubeFragPath);
    // shaders
    lightSourceShader.use();
    lightSourceShader.setVec3("lightColor", glm::vec3(1.0f));

    // importing models
    //const char* modelVertexPath = "C:/Users/river/source/repos/VoxelEngine/VoxelEngine/modelVertex.glsl";
    //const char *basicFragPath = "C://Users//river//CLionProjects//VoxelEngine//src//shaders//basicFrag.glsl";
    //const char *backpackPath = "C:/Users/river/source/repos/VoxelEngine/VoxelEngine/resources/objects/backpack/backpack.obj";
    //Shader basicShader = Shader(modelVertexPath, basicFragPath);
    //Model backpack(backpackPath);


    registerTextures(textures, paths, 3);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, textures[2]);

    glm::vec3 cubePos(0.0f);

    unsigned int VBOs[10], VAOs[10], EBOs[10];
    generateCube(VBOs, VAOs, EBOs);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_btn_callback);


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
    Chunk chunk;
    chunk.CreateMesh();


    while (!glfwWindowShouldClose(window))
    {
        // input
        updateDeltaTime();
        processInput(window);

        // imgui stuff
        SetupImGuiRenderLoop();

        // Clear buffers, set background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // rendering
        setShaderData(shader);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float) screenWidth / (float) screenHeight,
                                                0.1f, 100.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setMat4("model", model);
        shader.setInt("material.texture_diffuse1", 0);
        glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textures[0]);
        chunk.Render(shader);
        //myMesh.Draw(shader);



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

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
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
    glGenTextures(nTextures, texture);
    for (int i = 0; i < nTextures; i++)
    {
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

    }
}

void updateDeltaTime()
{
    currentFrame = (float) glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

//Mesh createMesh()
//{
//    std::vector<Vertex> vertices;
//    // face 1, annotated as face 1 vertex 1, face 1 vertex 2, etc
//    // position vec3 | normal vec3 | texcoord vec2
//    Vertex f1v1{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f1v2{ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f1v3{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f1v4{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f1v5{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f1v6{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)};
//
//    Vertex f2v1{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f2v2{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f2v3{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f2v4{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f2v5{ glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f2v6{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)};
//    
//    Vertex f3v1{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f3v2{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f3v3{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f3v4{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f3v5{ glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f3v6{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//
//    Vertex f4v1{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f4v2{ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f4v3{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f4v4{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f4v5{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f4v6{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//
//    Vertex f5v1{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f5v2{ glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f5v3{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f5v4{ glm::vec3(1.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f5v5{ glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f5v6{ glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//
//    Vertex f6v1{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//    Vertex f6v2{ glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)};
//    Vertex f6v3{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f6v4{ glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)};
//    Vertex f6v5{ glm::vec3(0.0f, 1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)};
//    Vertex f6v6{ glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)};
//
//    vertices.push_back(f1v1);
//    vertices.push_back(f1v2);
//    vertices.push_back(f1v3);
//    vertices.push_back(f1v4);
//    vertices.push_back(f1v5);
//    vertices.push_back(f1v6);
//
//    vertices.push_back(f2v1);
//    vertices.push_back(f2v2);
//    vertices.push_back(f2v3);
//    vertices.push_back(f2v4);
//    vertices.push_back(f2v5);
//    vertices.push_back(f2v6);
//
//    vertices.push_back(f3v1);
//    vertices.push_back(f3v2);
//    vertices.push_back(f3v3);
//    vertices.push_back(f3v4);
//    vertices.push_back(f3v5);
//    vertices.push_back(f3v6);
//
//    vertices.push_back(f4v1);
//    vertices.push_back(f4v2);
//    vertices.push_back(f4v3);
//    vertices.push_back(f4v4);
//    vertices.push_back(f4v5);
//    vertices.push_back(f4v6);
//
//    vertices.push_back(f5v1);
//    vertices.push_back(f5v2);
//    vertices.push_back(f5v3);
//    vertices.push_back(f5v4);
//    vertices.push_back(f1v5);
//    vertices.push_back(f5v6);
//
//    vertices.push_back(f6v1);
//    vertices.push_back(f6v2);
//    vertices.push_back(f6v3);
//    vertices.push_back(f6v4);
//    vertices.push_back(f6v5);
//    vertices.push_back(f6v6);
//
//    std::vector<unsigned int> indices;
//    for (int i = 0; i < 24; i++)
//		indices.push_back(i);
//
//    std::vector<Texture> texs;
//    Texture diffuse;
//    diffuse.id = textures[0];
//    std::cout << " diffuse id: " << diffuse.id << std::endl;
//    diffuse.type = "texture_diffuse";
//    texs.push_back(diffuse);
//
//    return Mesh(vertices, indices, texs);
//}

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
    shader.setVec3("viewPos", camera.Position);

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

    shader.setVec3("spotLight.position", camera.Position);
    shader.setVec3("spotLight.direction", camera.Front);
    shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    shader.setVec3("spotLight.specular", 0.3f, 0.3f, 0.3f);
    shader.setFloat("spotLight.constant", 1.0f);
    shader.setFloat("spotLight.linear", 0.09f);
    shader.setFloat("spotLight.quadratic", 0.032f);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
    shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));
}

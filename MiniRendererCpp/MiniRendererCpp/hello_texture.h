#pragma once
#include "glad.h"
#include "glfw3.h"
#include "shader.h"
#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
#include <glm/glm.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "ModelGL.h"
#include "camera.h"
#include <iostream>
#include <windows.h>
#include <commdlg.h>

void framebuffer_size_callback_texture(GLFWwindow* window, int width, int height);
void mouse_callback_texture(GLFWwindow* window, double xpos, double ypos);
void scroll_callback_texture(GLFWwindow* window, double xoffset, double yoffset);
void processInput_ibl (GLFWwindow* window);
void renderSphere_texture();
void renderQuad_texture();
unsigned int loadTexture_1(char const* path);
// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture_1(char const* path)
{
    stbi_set_flip_vertically_on_load(true);
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// settings
const unsigned int SCR_WIDTH_TEX = 1280;
const unsigned int SCR_HEIGHT_TEX = 720;

// camera
Camera camera_tex(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX_tex = 800.0f / 2.0;
float lastY_tex = 600.0 / 2.0;
bool firstMouse_tex = true;

// timing
float deltaTime_tex = 0.0f;
float lastFrame_tex = 0.0f;

int test_texture_attribute()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH_TEX, SCR_HEIGHT_TEX, "TestUVSpace", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_texture);
    glfwSetCursorPosCallback(window, mouse_callback_texture);
    glfwSetScrollCallback(window, scroll_callback_texture);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    Shader textureAttributeShader("shaders/chapter_texture/textureAttribute.vs", "shaders/chapter_texture/textureAttribute.fs");
    textureAttributeShader.use();
    textureAttributeShader.setInt("albedoMap", 0);
    unsigned int albedo = loadTexture_1("resources/textures/uv1.png");

    // imgui窗口设置
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.MouseDrawCursor = true; // 确保鼠标光标被绘制
    // 设置颜色主题
    ImGui::StyleColorsDark();
    //设置字体
    io.Fonts->AddFontFromFileTTF("fonts/Genshin.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    float tilingX = 1.0f;
    float tilingY = 1.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime_tex = currentFrame - lastFrame_tex;
        lastFrame_tex = currentFrame;
        // input
        // -----
        processInput_ibl(window);
        // 启动ImGui框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin(u8"纹理映射demo");
            ImGui::SetWindowSize(ImVec2(800, 500));
            ImGui::SetWindowPos(ImVec2(0, 0));
            //可以移动位置
            //ImGui::

            ImGui::Text(u8"控制tiling和offset");
            //ImGui::SliderFloat("tilingX", &tilingX, -10.0f, 10.0f);
            //ImGui::SameLine(); // 在同一行显示
            ImGui::InputFloat("##input", &tilingX, 0.1f, 1.0f, "%.3f");
            //ImGui::SliderFloat("tilingY", &tilingY, -10.0f, 10.0f);
            //ImGui::SameLine(); // 在同一行显示
            ImGui::InputFloat("##input2", &tilingY, 0.1f, 1.0f, "%.3f");
            //ImGui::SliderFloat("offsetX", &offsetX, -1.0f, 1.0f);
            //ImGui::SameLine(); // 在同一行显示
            ImGui::InputFloat("##input3", &offsetX, 0.05f, 1.0f, "%.3f");
            //ImGui::SliderFloat("offsetY", &offsetY, -1.0f, 1.0f);
            //ImGui::SameLine(); // 在同一行显示
            ImGui::InputFloat("##input4", &offsetY, 0.05f, 1.0f, "%.3f");

            ImGui::End();
        }

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        textureAttributeShader.use();

        //set value
        textureAttributeShader.setVec4("tilingAndOffset", glm::vec4(tilingX, tilingY, offsetX, offsetY));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        textureAttributeShader.setMat4("model", model);

        glm::mat4 projection = glm::perspective(glm::radians(camera_tex.Zoom), (float)SCR_WIDTH_TEX / (float)SCR_HEIGHT_TEX, 0.1f, 100.0f);
        glm::mat4 view = camera_tex.GetViewMatrix();
        textureAttributeShader.setMat4("projection", projection);
        textureAttributeShader.setMat4("view", view);
        renderQuad_texture();

        // 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // 清除GUI资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();

}

int test_UV_space()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH_TEX, SCR_HEIGHT_TEX, "TestUVSpace", NULL, NULL);
    glfwMakeContextCurrent(window);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback_texture);
    glfwSetCursorPosCallback(window, mouse_callback_texture);
    glfwSetScrollCallback(window, scroll_callback_texture);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    Shader textureTipShader("shaders/chapter_texture/textureTip.vs", "shaders/chapter_texture/textureTip.fs");
    textureTipShader.use();
    textureTipShader.setInt("albedoMap", 0);

    unsigned int albedo = loadTexture_1("resources/textures/uv1.png");
    
    // imgui窗口设置
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.MouseDrawCursor = true; // 确保鼠标光标被绘制
    // 设置颜色主题
    ImGui::StyleColorsDark();
    //设置字体
    io.Fonts->AddFontFromFileTTF("fonts/Genshin.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    float uvalue = 0;
    float vvalue = 0;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime_tex = currentFrame - lastFrame_tex;
        lastFrame_tex = currentFrame;
        // input
        // -----
        processInput_ibl(window);
        // 启动ImGui框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin(u8"纹理映射demo");
            ImGui::SetWindowSize(ImVec2(300, 500));
            ImGui::SetWindowPos(ImVec2(0, 400));

            ImGui::Text(u8"拖动UV坐标");
            //两个拖动条，用于控制UV坐标，范围0-1
            ImGui::SliderFloat("U", &uvalue, 0.0f, 1.0f);
            ImGui::SliderFloat("V", &vvalue, 0.0f, 1.0f);
            ImGui::End(); 
        }
            
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        textureTipShader.use();

        //set value
        textureTipShader.setVec2("uvLocation", glm::vec2(uvalue, vvalue));
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        textureTipShader.setMat4("model", model);

        glm::mat4 projection = glm::perspective(glm::radians(camera_tex.Zoom), (float)SCR_WIDTH_TEX / (float)SCR_HEIGHT_TEX, 0.1f, 100.0f);
        glm::mat4 view = camera_tex.GetViewMatrix();
        textureTipShader.setMat4("projection", projection);
        textureTipShader.setMat4("view", view);
        renderSphere_texture();

        // 渲染
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // 清除GUI资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}



unsigned int quadVAO_tex = 0;
unsigned int quadVBO_tex;
void renderQuad_texture()
{
    if (quadVAO_tex == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO_tex);
        glGenBuffers(1, &quadVBO_tex);
        glBindVertexArray(quadVAO_tex);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO_tex);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO_tex);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO_tex = 0;
unsigned int indexCount_tex;
void renderSphere_texture()
{
    if (sphereVAO_tex == 0)
    {
        glGenVertexArrays(1, &sphereVAO_tex);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount_tex = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO_tex);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO_tex);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount_tex, GL_UNSIGNED_INT, 0);
}

void framebuffer_size_callback_texture(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback_texture(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse_tex)
    {
        lastX_tex = xpos;
        lastY_tex = ypos;
        firstMouse_tex = false;
    }

    float xoffset = xpos - lastX_tex;
    float yoffset = lastY_tex - ypos; // reversed since y-coordinates go from bottom to top

    lastX_tex = xpos;
    lastY_tex = ypos;

    //camera_tex.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback_texture(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_tex.ProcessMouseScroll(static_cast<float>(yoffset));
}


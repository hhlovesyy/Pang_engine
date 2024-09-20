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

static float ls1_move_x = 0;
static float ls1_move_y = 0;
static float ls1_rotate_angle = 0;
static float ls1_scale_valueX = 1.0f;
static float ls1_scale_valueY = 1.0f;
static float ls1_deltaTime = 0.0f;
static float ls1_lastFrame = 0.0f;

void ls1_framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void ls1_processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

/*
* 一些笔记：关于三角形在旋转的时候会发生变形的问题，目前排查认为有两点原因可能导致这个问题：：
* 1. 在旋转的时候，屏幕坐标系和世界坐标系的问题，旋转的时候是绕着世界坐标系的Z轴旋转的，似乎会造成屏幕空间的拉伸问题，看一下最后perspective用的矩阵
* 2.应用变换矩阵的顺序问题，在应用的时候好像是先平移再旋转再缩放才对，不知道为什么，这个顺序应该也可以自己定义
*/
int test_triangle_rotate()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, ls1_framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // imgui窗口设置
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // 设置颜色主题
    ImGui::StyleColorsDark();
    //设置字体
    io.Fonts->AddFontFromFileTTF("fonts/Genshin.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Shader simpleShader("shaders/simpleVS.vs", "shaders/simpleFS.fs");

    float vertices[] = {
         -0.5f, -0.5f, 0.0f,  // top right
         0.0f, -0.5f, 0.0f,  // bottom right
        0.5f, -0.5f, 0.0f,  // bottom left
        0.0f,  0.5f, 0.0f   // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        //1, 2, 3   // second Triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //关闭深度缓冲
    glDisable(GL_DEPTH_TEST);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    //循环
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        ls1_deltaTime = currentFrame - ls1_lastFrame;
        ls1_lastFrame = currentFrame;
        ls1_processInput(window);
        // 启动ImGui框架
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            ImGui::Begin(u8"平移、旋转、缩放矩阵Demo（二维）");
            ImGui::SetWindowSize(ImVec2(500, 650));
            ImGui::SetWindowPos(ImVec2(0, 400));
            ImGui::Text(u8"平移参数");
            ImGui::SliderFloat(u8"平移 X", &ls1_move_x, -1.0f, 1.0f);
            ImGui::SliderFloat(u8"平移", &ls1_move_y, -1.0f, 1.0f);
            ImGui::Separator();
            ImGui::Text(u8"旋转参数(逆时针，绕Z轴)");
            ImGui::SliderFloat(u8"旋转角度", &ls1_rotate_angle, -180.0f, 180.0f);
            ImGui::Separator();
            ImGui::Text(u8"缩放参数");
            ImGui::SliderFloat(u8"缩放X", &ls1_scale_valueX, 0.1f, 2.0f);
            ImGui::SliderFloat(u8"缩放Y", &ls1_scale_valueY, 0.1f, 2.0f);
            ImGui::Separator();
            ImGui::Text(u8"对应的二维矩阵如下：");
            ImGui::Text(u8"平移矩阵：");
            ImGui::Text(u8"1\t\t0\t\tx\t\t=\t\t1\t\t0\t\t%.2f", ls1_move_x);
            ImGui::Text(u8"0\t\t1\t\ty\t\t=\t\t0\t\t1\t\t%.2f", ls1_move_y);
            ImGui::Text(u8"0\t\t0\t\t1\t\t=\t\t0\t\t0\t\t1");
            ImGui::Text(u8"旋转矩阵：");
            ImGui::Text(u8"cos(r)\t\t-sin(r)\t\t0\t\t=\t\t%.2f\t\t%.2f\t\t0", cos(glm::radians(ls1_rotate_angle)), -sin(glm::radians(ls1_rotate_angle)));
            ImGui::Text(u8"sin(r)\t\tcos(r)\t\t0\t\t=\t\t%.2f\t\t%.2f\t\t0", sin(glm::radians(ls1_rotate_angle)), cos(glm::radians(ls1_rotate_angle)));
            ImGui::Text(u8"0\t\t\t\t0\t\t\t\t1\t\t=\t\t\t0\t\t\t0\t\t\t1");
            ImGui::Text(u8"缩放矩阵：");
            ImGui::Text(u8"SX\t\t0\t\t0\t\t=\t\t%.2f\t\t0\t\t0",ls1_scale_valueX);
            ImGui::Text(u8"0\t\tSY\t\t0\t\t=\t\t0\t\t%.2f\t\t0", ls1_scale_valueY);
            ImGui::Text(u8"0\t\t0\t\t\t1\t\t=\t\t0\t\t0\t\t\t1");
            ImGui::Text(u8"平均帧率 %.3f ms/帧 (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        simpleShader.use();
        //矩阵运算，对顶点进行变换
        glm::mat4 model = glm::mat4(1.0f);
    
        model = glm::translate(model, glm::vec3(ls1_move_x, ls1_move_y, 0.0f));
        model = glm::rotate(model, glm::radians(ls1_rotate_angle), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(ls1_scale_valueX, ls1_scale_valueY, 1.0f));

        glm::mat4 view = glm::mat4(1.0f);
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
        //直接对顶点进行变换
        simpleShader.setMat4("model", model);
        simpleShader.setMat4("view", view);
        simpleShader.setMat4("projection", projection);

        glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    // 清除GUI资源
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}
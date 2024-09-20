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
#include "hello_triangle.h"
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool frozenInput = false;
bool altIsPressed = false;

bool light_varied = false;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
float light_rotationX = 0.0f; // ��ת�Ƕ�
float light_rotationY = 0.0f;

// һЩshader���Կ����Ĳ���
bool test_half_lambert = false;
bool show_diffuse = true;
bool show_specular = true;
bool show_ambient = true;

//�߹�ϵ��
float shininess = 32.0;


//void set_light()
//{
//    // ���ù�Դ����Դ������һ��������
//    float vertices[] = {
//        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//
//        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
//
//        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//
//         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//
//        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//
//        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
//         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
//         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
//    };
//    // ���� VAO �� VBO
//    unsigned int VBO, lightVAO;
//    glGenVertexArrays(1, &lightVAO);
//    glGenBuffers(1, &VBO);
//
//    // �� VBO �� VAO
//    glBindVertexArray(lightVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
//
//    // ��������ָ��
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // ��������
//    glEnableVertexAttribArray(0);
//
//    // ��� VBO �� VAO
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//}

void update_light_dir()
{
    // ����ת�Ƕ�ת��Ϊ���Ȳ����¹�Դλ��
    lightPos.x = cos(glm::radians(light_rotationY)) * cos(glm::radians(light_rotationX)) * 10.0f; // ������Ҫ��������
    lightPos.y = sin(glm::radians(light_rotationX)) * 10.0f;
    lightPos.z = sin(glm::radians(light_rotationY)) * cos(glm::radians(light_rotationX)) * 10.0f;
}

//���µ�����������ʱ�ò��ˣ����������Ļ�������
//std::string ConvertLPWSTRToString(LPWSTR lpwstr)
//{
//    // ��ȡ����Ļ�������С
//    int size_needed = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, NULL, 0, NULL, NULL);
//    // ����һ�� std::string �����仺����
//    std::string str(size_needed, 0);
//    // �� LPWSTR ת��Ϊ std::string
//    WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, &str[0], size_needed, NULL, NULL);
//    return str;
//}
//
//void ChooseObjFile(Model& ourModel, Shader& ourShader, Shader& lightShader)
//{
//    OPENFILENAMEW ofn;       // ���ڴ��ļ��Ի���
//    wchar_t szFile[260];     // �洢ѡ����ļ�·����ʹ�ÿ��ַ�
//
//    // ��ʼ�� OPENFILENAME �ṹ��
//    ZeroMemory(&ofn, sizeof(ofn));
//    ofn.lStructSize = sizeof(ofn);
//    ofn.hwndOwner = NULL;
//    ofn.lpstrFile = szFile;  // ʹ�ÿ��ַ�������
//    ofn.lpstrFile[0] = L'\0'; // ��ʼ��Ϊ��
//    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t); // ����ļ�������
//    ofn.lpstrFilter = L"OBJ Files (*.obj)\0*.obj\0All Files (*.*)\0*.*\0"; // �ļ�������
//    ofn.nFilterIndex = 1;
//    ofn.lpstrFileTitle = NULL;
//    ofn.nMaxFileTitle = 0;
//    ofn.lpstrInitialDir = NULL;
//    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
//    //Ĭ��·��Ϊ��ǰ�ļ���·��
//    GetCurrentDirectoryW(260, ofn.lpstrFile);
//
//    // ��ʾ�ļ��Ի���
//    if (GetOpenFileNameW(&ofn) == TRUE) // ʹ�ÿ��ַ��汾�ĺ���
//    {
//        // �����ѡ����ļ�·��
//        std::wcout << L"Selected file: " << ofn.lpstrFile << std::endl; // ʹ�ÿ��ַ����
//        std::string filePath = ConvertLPWSTRToString(ofn.lpstrFile);
//
//        //�ͷ�ԭ��ģ����Դ
//        ourModel = Model(filePath);
//        //��������shader
//        ourShader = Shader("shaders/testVS.vs", "shaders/testFS.fs");
//        lightShader = Shader("shaders/lightVS.vs", "shaders/lightFS.fs");
//    }
//}

void reset_everything()
{
    light_rotationX = 0.0f;
	light_rotationY = 0.0f;
	test_half_lambert = false;
	show_diffuse = true;
	show_specular = true;
	show_ambient = true;
	shininess = 32.0;
}

int test_blinn_phong()
{
    // glfw: initialize and configure
// ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    //stbi_set_flip_vertically_on_load(true);  //����������ģ�ͣ�����Ҫ��ת����
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // imgui��������
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    // ������ɫ����
    ImGui::StyleColorsDark();
    //��������
    io.Fonts->AddFontFromFileTTF("fonts/Genshin.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaders/testVS.vs", "shaders/testFS.fs");
    Shader lightShader("shaders/lightVS.vs", "shaders/lightFS.fs");
    Model ourModel("obj/LaiKaEn.obj");

    //set_light();

    ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    bool default_color = true;


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        processInput(window);
        // ����ImGui���
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        {
            //static float f = 0.0f;
            ImGui::Begin(u8"��Ⱦ��");
            ImGui::SetWindowSize(ImVec2(500, 500));
            ImGui::SetWindowPos(ImVec2(0, 400));

            ImGui::Text(u8"��Դ�������");
            // ʹ�û������ X, Y ��ת�Ƕȣ���ΧΪ 0 �� 360��
            ImGui::SliderFloat("Rotation X", &light_rotationX, 0.0f, 360.0f);
            ImGui::SliderFloat("Rotation Y", &light_rotationY, 0.0f, 360.0f);
            // ����ָ���
            ImGui::Separator();
            ImGui::Text(u8"�鿴ÿһ��");
            // ��ѡ��ѡ����ʹ�����α�����ɫ����ɫ��ȡ����ѡ��ν���ȫ����ɫ����
            ImGui::Checkbox(u8"ʹ��half-lambert", &test_half_lambert);
            ImGui::Checkbox(u8"��ʾ������", &show_diffuse);
            ImGui::Checkbox(u8"��ʾ���淴��", &show_specular);
            ImGui::Checkbox(u8"��ʾ������", &show_ambient);

            // �߹�ϵ����slider��2 ~ 256
            ImGui::Separator();
            ImGui::SliderFloat(u8"�߹�ϵ��", &shininess, 2.0, 256.0);

            // ����һ����ť
            ImGui::Separator();
            if (ImGui::Button(u8"�������в���"))
            {
                // �����ť����������ûص�����
                //ChooseObjFile(ourModel, ourShader, lightShader);
                reset_everything();
            }
            ImGui::Separator();
            ImGui::Checkbox(u8"���Ա任��Դ��ɫ����", &light_varied);


            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        update_light_dir();
        // render
        // ------
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // render container, don't forget to enable shader before setting uniforms
        ourShader.use();
        ourShader.setVec3("light.position", lightPos);
        ourShader.setVec3("viewPos", camera.Position);
        //��//0:show ambient 1:show diffuse 2.show specular 3:is half lambert
        ourShader.setVec4("showThing", show_ambient, show_diffuse, show_specular, test_half_lambert);

        glm::vec3 lightColor;
        if (light_varied)
        {
            lightColor.x = static_cast<float>(sin(glfwGetTime() * 2.0));
            lightColor.y = static_cast<float>(sin(glfwGetTime() * 0.7));
            lightColor.z = static_cast<float>(sin(glfwGetTime() * 1.3));
        }
        else
        {
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }

        glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
        glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f); // low influence
        ourShader.setVec3("light.ambient", ambientColor);
        ourShader.setVec3("light.diffuse", diffuseColor);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
        ourShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
        ourShader.setFloat("material.shininess", shininess);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);


        // ��Ⱦ
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------


    // ���GUI��Դ
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
}

int main()
{
    //int res = test_blinn_phong();
    int res = test_triangle_rotate();
    if (res == -1) return -1;
    else return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
		altIsPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
	{
        altIsPressed = false;
	}

    if (!frozenInput)
    {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    if (frozenInput || !altIsPressed) return;
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (frozenInput) return;
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
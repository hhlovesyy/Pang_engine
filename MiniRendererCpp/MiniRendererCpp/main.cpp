#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"
#define NOMINMAX
#include <windows.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr int width = 2048; // output image size
constexpr int height = 2048;

vec3 light_dir{ 0,0,1 }; // light source，指的是指向光源的方向
const vec3       eye{ 0,0,0.8 }; // camera position
const vec3    center{ 0,1.5,0 }; // 相机要看向的点
const vec3        up{ 0,1,0 }; // camera up vector
//std::vector<vec3> eye_new{ { 0,1.5,0.5 } , { 0,0.2,-0.7 } ,{ 0.1,0.8,0.7 } ,{ -0.1,0.7,0.7 } ,{ 0,1,0.8 },
//    { 0,1,1.2 } ,{ -0.3,0.8,0.8 } ,{ 0.3,0.8,0.8 } ,{ -0.3,0.8,-0.8 } ,{ -0.3,0.9,0.8 } };
std::vector<vec3> eye_new{ { 0,1.5,0.5 } , { -0.1,1.5,0.7 } ,{ 0.1,0.8,0.7 } ,{ -0.1,1.5,0.7 } ,{ 0,1,0.8 },
    { 0,1,1.2 } ,{ -0.3,0.8,0.8 } ,{ 0.3,1.5,0.8 } ,{ -0.3,2,-0.8 } ,{ -0.3,1.5,0.8 } };
extern mat<4, 4> ModelView; // "OpenGL" state matrices
extern mat<4, 4> Projection;


TGAColor sample2D(const TGAImage& texture, vec2 uv);

struct Shader : IShader
{
    const Model& model;
    mat<2, 3> varying_uv;  // triangle uv coordinates, written by the vertex shader, read by the fragment shader
    mat<3, 3> varying_nrm; // normal per vertex to be interpolated by FS
    mat<3, 3> varying_viewDirection;
    mat<3, 3> view_tri;    // triangle in view coordinates
    vec3 uniform_l;       // light direction in view coordinates，暂时就是世界空间就行

    Shader(const Model& m) : model(m)
    {
        uniform_l = light_dir.normalized() * 1.0;
    }

    virtual void vertex(const int iface, const int nthvert, vec4& gl_Position, bool& should_discard, int eye_index)
    {
        should_discard = false;
        //      vec3 viewDirection = eye_new[eye_index] - model.vert(iface, nthvert);
        //      vec3 ABnormal = model.vert(iface, 1) - model.vert(iface, 0);
        //      vec3 ACnormal = model.vert(iface, 2) - model.vert(iface, 0);
        //      vec3 normal = cross(ABnormal, ACnormal).normalized();
        ////      //都搞到相机空间当中
        //      vec3 viewSpaceNormal = proj<3>((ModelView).invert_transpose() * embed<4>(normal, 0.)).normalized();
        //      vec3 viewSpaceViewDirection = proj<3>(ModelView * embed<4>(viewDirection)).normalized();
        //      if (viewSpaceNormal * viewSpaceViewDirection < -1e-6)  //背面剔除
        //      {
              //	should_discard = true;
              //	return;  //这段代码目前是错误的，todo:后面debug一下法线的方向
           //   }

        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        //varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
        varying_nrm.set_col(nthvert, model.normal(iface, nthvert)); //法线在世界空间当中
        varying_viewDirection.set_col(nthvert, eye_new[eye_index] - model.vert(iface, nthvert));
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert)); //现在转到的是相机空间
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection * gl_Position;  //现在转到的是裁剪空间，还没做透视除法
    }

    virtual bool fragment(const vec3 bar, TGAColor& gl_FragColor, int face_index)
    {
        vec3 bn = (varying_nrm * bar).normalized(); // per-vertex normal interpolation
        vec2 uv = varying_uv * bar; // tex coord interpolation
        //目前来说光照做在了世界空间当中
        double diff = std::max(0., bn * uniform_l); // diffuse light intensity
        //来一波背面剔除
        //todo:优化：改为三角形剔除而不是像素剔除
        vec3 viewDirection = (varying_viewDirection * bar).normalized();
        diff = diff * 0.5 + 0.5; //half lambert
        //if(bn * viewDirection < 0)
             //return true; // discard the pixel if it's a back face (with respect to the camera
        TGAColor c = sample2D(model.diffuse(face_index), uv);
        for (int i : {0, 1, 2})
        {
            gl_FragColor[i] = std::min<int>(10 + c[i] * diff, 255);  //10指的是环境光，暂时先写死进去
            //gl_FragColor[i] = c[i];  //todo：暂时先不考虑光照
            //gl_FragColor[i] = 255;
        }


        return false; // the pixel is not discarded
    }
};

TGAColor sample2D(const TGAImage& texture, vec2 uv)
{
    return texture.get(uv.x * texture.width(), uv.y * texture.height());
}

void WriteZBufferToFile(std::vector<double>& zbuffer, std::string& filename)
{
    TGAImage zbuffer_image(width, height, TGAImage::RGBA);
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            double z_value = zbuffer[i + j * width];
            //-1~1 =>0~1
            z_value = (z_value + 1) / 2;
            /*if(z_value < 100)
                std::cout << z_value << std::endl;*/
            TGAColor color = TGAColor{ static_cast<std::uint8_t>(z_value * 255), static_cast<std::uint8_t>(z_value * 255), static_cast<std::uint8_t>(z_value * 255), 255, 32 };
            zbuffer_image.set(i, j, color);
        }
    }
    zbuffer_image.write_tga_file(filename);
}

vec3 calculate_around_eyepos(int index)
{
    //以eye{ 0,0,0.8 }为第一个，围绕中心center{ 0,1.5,0 }旋转，半径为1.5，高度为0.8，输入index，输出eye的位置，一共20个
    double angle = 2 * 3.1415926 / 10 * index;
    double x = 0 + 1.2 * cos(angle);
    double z = 0 + 1.2 * sin(angle);
    double y = 0.8;
    return vec3{ x,y,z };

}
int ssaa_sample = 3;
int ssaa_2 = ssaa_sample * ssaa_sample;
void RenderModel()
{
    //Model model("obj/african_head.obj", "african_head"); // load an object
    //Model model("obj/RobinFix.obj", "Robin");
    //Model model("obj/Pamu.obj", "Pamu");
    Model model("obj/LaiKaEn.obj", "LaiKaEn");
    Shader shader(model);

    for (int index = 0; index < 10; index++)  //不同角度渲染10张图，看看效果
    {
        TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
        std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
        //MSAA_zbuffer
        std::vector<double> MSAA_zbuffer(width * height * ssaa_2, std::numeric_limits<double>::max());
        std::vector<TGAColor > fram_buf_ssaa(width * height * ssaa_2, TGAColor{ 0,0,0,0 });
        std::vector<double> depth_buf_ssaa(width * height * ssaa_2, std::numeric_limits<double>::max());

        lookat(calculate_around_eyepos(index), center, up); //计算model-view矩阵
        //viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
        viewport(0, 0, width, height); // build the Viewport matrix
        //projection(-1, 1, -1, 1, 0.3, 1); // build the Projection matrix
        projection(60, 1, 0.01, 1); // build the Projection matrix
        int face_number = model.nfaces();
        int startIndex = 0; //修改这两个用于Debug
        //startIndex = 7032;
        //face_number = 26712;

        for (int i = startIndex; i < face_number; i++)  // for every triangle
        {
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            bool discard = false;  //后面做一下背面剔除
            for (int j : {0, 1, 2})
            {
                shader.vertex(i, j, clip_vert[j], discard, index); // call the vertex shader for each triangle vertex
            }
            if (discard) continue;
            triangle(clip_vert, shader, framebuffer, zbuffer,MSAA_zbuffer, fram_buf_ssaa,depth_buf_ssaa,i); // the core rasterizer
        }
        std::string name = std::to_string(index) + "RobinResFinal.tga";
        std::string zbuffer_name = std::to_string(index) + "RobinZBuffer.tga";
        framebuffer.write_tga_file(name);
        WriteZBufferToFile(zbuffer, zbuffer_name);

    }
    std::cout << "finish rendering!" << std::endl;
}

//int main(int argc, char** argv)
//{
//    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
//    std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
//    //RenderJustTriangle(framebuffer, zbuffer);
//    RenderModel(framebuffer, zbuffer);
//    return 0;
//}

void main_renderer()
{
    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
    std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
    //MSAA_zbuffer
    int ssaa_sample = 3;
    int ssaa_2 = ssaa_sample * ssaa_sample;
    std::vector<double> MSAA_zbuffer(width * height* ssaa_2, std::numeric_limits<double>::max());
    std::vector<TGAColor > fram_buf_ssaa(width * height * ssaa_2, TGAColor{ 0,0,0,0 });
    std::vector<double> depth_buf_ssaa(width * height * ssaa_2, std::numeric_limits<double>::max());
    //RenderJustTriangle(framebuffer, zbuffer);
    RenderModel();
}

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <GL/GL.h>
#include <tchar.h>

// Data stored per platform window
struct WGL_WindowData { HDC hDC; };

// Data
static HGLRC            g_hRC;
static WGL_WindowData   g_MainWindow;
static int              g_Width;
static int              g_Height;

// Forward declarations of helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data);
void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data);
void ResetDeviceWGL();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int main(int, char**)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_OWNDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui Win32+OpenGL3 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize OpenGL
    if (!CreateDeviceWGL(hwnd, &g_MainWindow))
    {
        CleanupDeviceWGL(hwnd, &g_MainWindow);
        ::DestroyWindow(hwnd);
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    wglMakeCurrent(g_MainWindow.hDC, g_hRC);

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_InitForOpenGL(hwnd);
    ImGui_ImplOpenGL3_Init();

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button to render"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            {
                counter++;
                main_renderer();
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        glViewport(0, 0, g_Width, g_Height);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Present
        ::SwapBuffers(g_MainWindow.hDC);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceWGL(hwnd, &g_MainWindow);
    wglDeleteContext(g_hRC);
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    HDC hDc = ::GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd = { 0 };
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;

    const int pf = ::ChoosePixelFormat(hDc, &pfd);
    if (pf == 0)
        return false;
    if (::SetPixelFormat(hDc, pf, &pfd) == FALSE)
        return false;
    ::ReleaseDC(hWnd, hDc);

    data->hDC = ::GetDC(hWnd);
    if (!g_hRC)
        g_hRC = wglCreateContext(data->hDC);
    return true;
}

void CleanupDeviceWGL(HWND hWnd, WGL_WindowData* data)
{
    wglMakeCurrent(nullptr, nullptr);
    ::ReleaseDC(hWnd, data->hDC);
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            g_Width = LOWORD(lParam);
            g_Height = HIWORD(lParam);
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


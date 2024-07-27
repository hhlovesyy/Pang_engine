#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

constexpr int width = 800; // output image size
constexpr int height = 800;

vec3 light_dir{ 0,0,1 }; // light source，指的是指向光源的方向
const vec3       eye{ 0,0,0.8 }; // camera position
const vec3    center{ 0,1.2,0 }; // 相机要看向的点
const vec3        up{ 0,1,0 }; // camera up vector

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

    virtual void vertex(const int iface, const int nthvert, vec4& gl_Position, bool& should_discard)
    {
        should_discard = false;
  //      vec3 viewDirection = eye - model.vert(iface, nthvert);
  //      vec3 ABnormal = model.vert(iface, 0) - model.vert(iface, 1);
  //      vec3 ACnormal = model.vert(iface, 0) - model.vert(iface, 2);
  //      vec3 normal = cross(ABnormal, ACnormal).normalized();
  //      //都搞到相机空间当中
  //      vec3 viewSpaceNormal = proj<3>((ModelView).invert_transpose() * embed<4>(normal, 0.)).normalized();
  //      vec3 viewSpaceViewDirection = proj<3>(ModelView * embed<4>(viewDirection)).normalized();
  //      if (viewSpaceNormal * viewSpaceViewDirection < 0)  //背面剔除
  //      {
		//	should_discard = true;
		//	return;
		//}

        varying_uv.set_col(nthvert, model.uv(iface, nthvert));
        //varying_nrm.set_col(nthvert, proj<3>((ModelView).invert_transpose() * embed<4>(model.normal(iface, nthvert), 0.)));
        varying_nrm.set_col(nthvert, model.normal(iface, nthvert)); //法线在世界空间当中
        varying_viewDirection.set_col(nthvert, eye - model.vert(iface, nthvert));
        gl_Position = ModelView * embed<4>(model.vert(iface, nthvert)); //现在转到的是相机空间
        view_tri.set_col(nthvert, proj<3>(gl_Position));
        gl_Position = Projection * gl_Position;  //现在转到的是NDC空间
    }

    virtual bool fragment(const vec3 bar, TGAColor& gl_FragColor, int face_index) 
    {
        vec3 bn = (varying_nrm * bar).normalized(); // per-vertex normal interpolation
        vec2 uv = varying_uv * bar; // tex coord interpolation
        //目前来说光照做在了世界空间当中
        double diff = std::max(0., bn * uniform_l); // diffuse light intensity

        TGAColor c = sample2D(model.diffuse(face_index), uv);
        //if (c[0] < 5 && c[1] < 5 && c[2] < 5)
        //{
        //    //std::cout << "something wrong with uv sampling\n" << std::endl;
        //    std::cout<<face_index<<std::endl;
        //    std::cout<<uv.x<<" "<<uv.y<<std::endl;
        //}
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

void RenderModel(TGAImage& framebuffer, std::vector<double>& zbuffer)
{
    //Model model("obj/african_head.obj", "african_head"); // load an object
    //Model model("obj/RobinFix.obj", "Robin");
    //Model model("obj/Pamu.obj", "Pamu");
    Model model("obj/LaiKaEn.obj", "Pamu");
    Shader shader(model);
    
    std::vector<vec3> eye_new{ { 0,1.5,0.8 } , { 0,0.2,-0.8 } ,{ 0.1,0.8,0.8 } ,{ -0.1,0.7,0.8 } ,{ 0,1,0.8 },
    { 0,1,1.2 } ,{ -0.3,0.8,0.8 } ,{ 0.3,0.8,0.8 } ,{ -0.3,0.8,-0.8 } ,{ -0.3,0.9,0.8 } };
    for (int index = 0; index < 10; index++)  //不同角度渲染10张图，看看效果
    {
        TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
        std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
        lookat(eye_new[index], center, up); //计算model-view矩阵
        //viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4); // build the Viewport matrix
        viewport(0, 0, width, height); // build the Viewport matrix
        projection(-1, 1, -1, 1, 0.8, 1); // build the Projection matrix

        int face_number = model.nfaces();
        int startIndex = 0; //修改这两个用于Debug
        
        for (int i = startIndex; i < face_number; i++)  // for every triangle
        {
            vec4 clip_vert[3]; // triangle coordinates (clip coordinates), written by VS, read by FS
            bool discard = false;  //做一下背面剔除
            for (int j : {0, 1, 2})
            {
                shader.vertex(i, j, clip_vert[j], discard); // call the vertex shader for each triangle vertex
            }
            if (discard) continue;
            triangle(clip_vert, shader, framebuffer, zbuffer, i);
        }
        std::string name = std::to_string(index) + "RobinResFinal.tga";
        framebuffer.write_tga_file(name);
    }
    
}

int main(int argc, char** argv)
{
    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
    std::vector<double> zbuffer(width * height, std::numeric_limits<double>::max());
    //RenderJustTriangle(framebuffer, zbuffer);
    RenderModel(framebuffer, zbuffer);
    return 0;
}

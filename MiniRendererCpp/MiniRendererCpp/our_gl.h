#include "tgaimage.h"
#include "geometry.h"

void viewport(const int x, const int y, const int w, const int h);
void projection(double left, double right, double bottom, double up, double near, double far); 
void lookat(const vec3 eye, const vec3 center, const vec3 up);

struct IShader 
{
    static TGAColor sample2D(const TGAImage& img, vec2& uvf) 
    {
        if(uvf[0] < 0) uvf[0] = 1 + uvf[0]; //有些神奇的插件会导致uv坐标为负数，这里做一下修正
        if(uvf[1] < 0) uvf[1] = 1 + uvf[1];
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const vec3 bar, TGAColor& color, int faceIndex) = 0;
};

void triangle(const vec4 clip_verts[3], IShader& shader, TGAImage& image, std::vector<double>& zbuffer, int face_index);
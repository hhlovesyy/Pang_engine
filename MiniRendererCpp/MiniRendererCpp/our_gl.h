#include "tgaimage.h"
#include "geometry.h"
#include "msaa_data.h"

// ȫ�ֱ������� �Ƿ�����SSAA
extern bool g_ssaaEnabled;
// ȫ�ֱ������� �Ƿ�����MSAA
extern bool g_msaaEnabled;

extern int ssaa_sample;
extern int ssaa_2;
extern int msaa_sample;//2*2
extern int msaa_sample_2;//2*2

void viewport(const int x, const int y, const int w, const int h);
void projection(double left, double right, double bottom, double up, double near, double far); 
void projection(double fovy, double aspect, double near, double far);
void lookat(const vec3 eye, const vec3 center, const vec3 up);

struct IShader 
{
    static TGAColor sample2D(const TGAImage& img, vec2& uvf) 
    {
        if(uvf[0] < 0) uvf[0] = 1 + uvf[0]; //��Щ����Ĳ���ᵼ��uv����Ϊ������������һ������
        if(uvf[1] < 0) uvf[1] = 1 + uvf[1];
        return img.get(uvf[0] * img.width(), uvf[1] * img.height());
    }
    virtual bool fragment(const vec3 bar, TGAColor& color, int faceIndex) = 0;
};

void triangle(const vec4 clip_verts[3], IShader& shader, TGAImage& image, std::vector<double>& zbuffer, std::vector<double>& MSAA_zbuffer, std::vector<TGAColor >& SSAA_framebuffer, std::vector<double>& SSAA_zbuffer, std::vector< msaaData >& MsaaData, int face_index);
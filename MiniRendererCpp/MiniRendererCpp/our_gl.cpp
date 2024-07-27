#include "our_gl.h"

mat<4, 4> ModelView;
mat<4, 4> Viewport;
mat<4, 4> Projection;

void lookat(const vec3 eye, const vec3 center, const vec3 up)
{
	// ������������model-view����
	vec3 z = (center - eye).normalized();  //look-at direction
	vec3 x = cross(z, up).normalized();
	vec3 y = cross(x, z).normalized();
	mat<4, 4> Minv = { {{x.x,x.y,x.z,0},   {y.x,y.y,y.z,0},   {-z.x,-z.y,-z.z,0},   {0,0,0,1}} };
	mat<4, 4> Tr = { {{1,0,0,-eye.x}, {0,1,0,-eye.y}, {0,0,1,-eye.z}, {0,0,0,1}} };
	ModelView = Minv * Tr;
}

//����о�����һ�㴴�£�viewport��λ�ü���һ��ƫ��ֵ��ȥ��������ʾ����Ļ�ϵ����ɶȸ�����һ��
void viewport(const int x, const int y, const int w, const int h) 
{
	Viewport = { {{w / 2., 0, 0, x + w / 2.}, {0, h / 2., 0, y + h / 2.}, {0,0,1,0}, {0,0,0,1}} };
}

void projection(double left, double right, double bottom, double top, double near, double far) //near and far should be above 0
{ 
	double n = near, f = far;
	Projection = { {{2 * n / (right - left), 0, (right + left) / (right - left), 0},
                    {0, 2 * n / (top - bottom), (top + bottom)/ (top - bottom), 0},
					{0, 0, (f + n) / (n - f), 2 * f * n / (n - f)},
					{0, 0, -1, 0} } };
}

vec3 barycentric(vec2* pts, vec2 P)  // pts[3] and P are in screen coordinates
{
    vec3 u = cross(vec3(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]), vec3(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z) < 1) return vec3(-1, 1, 1);
    return vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(const vec4 clip_verts[3], IShader& shader, TGAImage& image, std::vector<double>& zbuffer, int face_index)
{
    //clip_verts��NDC�ռ�������ζ�������,��û����͸�ӳ���
    
	//��������ӳ�䣬����͸�ӳ�����Ӧ��Ҳûʲôë��
	//vec4 pts[3] = { Viewport * clip_verts[0],    Viewport * clip_verts[1],    Viewport * clip_verts[2] };  // triangle screen coordinates before persp. division
	//vec2 pts2[3] = { proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]) };  // triangle screen coordinates after  perps. division
    
    //����͸�ӳ����ɣ�Ȼ�������ӿڱ任
    vec4 pts[3] = { clip_verts[0] / clip_verts[0][3], clip_verts[1] / clip_verts[1][3], clip_verts[2] / clip_verts[2][3] };  // triangle screen coordinates before persp. division
    vec4 tmpPts2[3] = { Viewport * pts[0],    Viewport * pts[1],    Viewport * pts[2] };  // triangle screen coordinates after  perps. division
    vec2 pts2[3] = { proj<2>(tmpPts2[0]), proj<2>(tmpPts2[1]), proj<2>(tmpPts2[2]) };  // triangle screen coordinates after  perps. division
    int bboxmin[2] = { image.width() - 1, image.height() - 1 };
    int bboxmax[2] = { 0, 0 };
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 2; j++) {
            bboxmin[j] = std::min(bboxmin[j], static_cast<int>(pts2[i][j]));
            bboxmax[j] = std::max(bboxmax[j], static_cast<int>(pts2[i][j]));
        }
#pragma omp parallel for
    for (int x = std::max(bboxmin[0], 0); x <= std::min(bboxmax[0], image.width() - 1); x++)
    {
        for (int y = std::max(bboxmin[1], 0); y <= std::min(bboxmax[1], image.height() - 1); y++) 
        {
            //������һ��4��SSAA��һ��
            //std::cout << x <<" "<< y << std::endl; x��y��������

            //refs:https://blog.csdn.net/Motarookie/article/details/124284471
            vec3 bc_screen = barycentric(pts2, { static_cast<double>(x), static_cast<double>(y) });
            //bc_screen ����Ļ�ռ����������������꣬Ҳ���������еĦ�'����'����'
            //vec3 bc_clip = { bc_screen.x / pts[0][3], bc_screen.y / pts[1][3], bc_screen.z / pts[2][3] };
            ////bc_clip.x + bc_clip.y + bc_clip.z �൱��1/z,���ο����ӡ�z����Ļ�ռ�ĵ�P����Ӧ��ͶӰǰ�ĵ�����ֵ����������Ŀ��ֵ
            //bc_clip = bc_clip / (bc_clip.x + bc_clip.y + bc_clip.z); 
            ////bc_clip�൱�����¼���֮�����ȷ���������꣬����Ƚ��ƣ�һ��Ҫ��Ӧ���������еĹ�ʽ����
            float w_reciprocal = 1.0 / (bc_screen.x / clip_verts[0][3] + bc_screen.y / clip_verts[1][3] + bc_screen.z / clip_verts[2][3]);
            // ���������������������Բ�ֵ��ʽ�� ����IAȡ��v[0].z()
            vec3  bc_clip = w_reciprocal * vec3(bc_screen.x / clip_verts[0][3] , bc_screen.y /clip_verts[1][3] , bc_screen.z / clip_verts[2][3]);
            double frag_depth = vec3{ clip_verts[0][2], clip_verts[1][2], clip_verts[2][2] }*bc_clip;
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z<0 || frag_depth > zbuffer[x + y * image.width()]) continue;
            TGAColor color;
            if (shader.fragment(bc_clip, color, face_index)) continue; // fragment shader can discard current fragment
            zbuffer[x + y * image.width()] = frag_depth;
            image.set(x, y, color);
        }
    }
}

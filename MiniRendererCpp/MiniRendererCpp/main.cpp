#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

constexpr int width = 800; // output image size
constexpr int height = 800;

vec3 barycentric(vec2* pts, vec2 P) 
{
    vec3 u = cross(vec3(pts[2][0] - pts[0][0], pts[1][0] - pts[0][0], pts[0][0] - P[0]) , vec3(pts[2][1] - pts[0][1], pts[1][1] - pts[0][1], pts[0][1] - P[1]));
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z) < 1) return vec3(-1, 1, 1);
    return vec3(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
}

void triangle(vec2 pts[3], TGAImage& image, TGAColor color)
{
    vec2 bboxmin(image.width() - 1, image.height() - 1);
    vec2 bboxmax(0, 0);
    vec2 clamp(image.width() - 1, image.height() - 1);
    for (int i = 0; i < 3; i++) {
        bboxmin.x = std::max(0.0, std::min(bboxmin.x, pts[i].x));
        bboxmin.y = std::max(0.0, std::min(bboxmin.y, pts[i].y));

        bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
        bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
    }
    vec2 P;
    for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) 
    {
        for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) 
        {
            vec3 bc_screen = barycentric(pts, P);
            if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
            image.set(P.x, P.y, color);
        }
    }

}

//blue：TGAColor(255, 0, 0
TGAColor red{ 255, 0, 0, 255, 4 };

void RenderJustTriangle(TGAImage& framebuffer)
{
    vec2 pts[3] = { vec2(10,10), vec2(700, 60), vec2(700, 560) };
    triangle(pts, framebuffer, red);
    framebuffer.write_tga_file("simpleTriangle.tga");
}

vec3 light_dir(0, 0, -1);
void RenderModel(TGAImage& framebuffer)
{
    //Model model("obj/african_head.obj"); // load an object
    Model model("obj/Robin.obj");
    for (int i = 0; i < model.nfaces(); i++)  // for every triangle
    {
        vec3 vert_pos[3];
        vec3 world_coords[3];
        vec2 screen_coords[3];
        for (int j : {0, 1, 2})
        {
            vert_pos[j] = model.vert(i, j);
            screen_coords[j] = vec2(static_cast<int>((vert_pos[j].x + 1.) * width / 2.), static_cast<int>((vert_pos[j].y + 1.) * height / 2. - 270)); //暂时完全不考虑z的问题，也不考虑空间变换
            world_coords[j] = vert_pos[j];
        }
        vec3 n = cross((world_coords[2] - world_coords[0]) , (world_coords[1] - world_coords[0]));
        n = n.normalized();
        float intensity = n * light_dir;
        if (intensity > 0) 
        {
            TGAColor color{ intensity * 255, intensity * 255, intensity * 255, 255, 4 };
            triangle(screen_coords, framebuffer, color);
        }
    }
    framebuffer.write_tga_file("huangquanwu.tga");
}

int main(int argc, char** argv) {
    TGAImage framebuffer(width, height, TGAImage::RGB); // the output image
    //RenderJustTriangle(framebuffer);
    RenderModel(framebuffer);
    return 0;
}

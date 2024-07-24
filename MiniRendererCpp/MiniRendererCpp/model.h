#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
    std::vector<vec3> verts{};     // array of vertices
    std::vector<vec2> tex_coord{}; // per-vertex array of tex coords
    std::vector<vec3> norms{};     // per-vertex array of normal vectors
    std::vector<int> facet_vrt{};// specular map texture
    std::vector<int> facet_tex{};  // per-triangle indices in the above arrays
    std::vector<int> submesh_idx{};
    std::vector<int> facet_nrm{};

    std::vector<std::string> submesh_name{};
    void load_texture(const std::string filename, const std::string suffix, std::vector<TGAImage>& img);
    std::vector<TGAImage> diffusemap{};
    std::string modelname;
public:
    Model(const std::string filename, const std::string modelname);
    int nverts() const;
    int nfaces() const;
    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const;
    vec2 uv(const int iface, const int nthvert) const;
    vec3 normal(const int iface, const int nthvert) const; // per triangle corner normal vertex
    const TGAImage& diffuse(int faceIndex)  const;
};


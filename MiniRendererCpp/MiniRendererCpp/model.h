#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model {
    std::vector<vec3> verts{};     // array of vertices
    std::vector<int> facet_vrt{};// specular map texture
public:
    Model(const std::string filename);
    int nverts() const;
    int nfaces() const;
    vec3 vert(const int i) const;
    vec3 vert(const int iface, const int nthvert) const;
};


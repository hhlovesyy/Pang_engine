#include <iostream>
#include <sstream>
#include "model.h"

Model::Model(const std::string filename, const std::string modelname) {
    this->modelname = modelname;
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            vec3 v;
            for (int i = 0; i < 3; i++) iss >> v[i];
            verts.push_back(v);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash >> trash;
            vec2 uv;
            for (int i = 0; i < 2; i++) iss >> uv[i];
            tex_coord.push_back({ uv.x, 1 - uv.y });
        }
        else if (!line.compare(0, 3, "vn "))
        {
            iss >> trash >> trash;
            vec3 n;
            for (int i = 0; i < 3; i++) iss >> n[i];
            norms.push_back(n.normalized());
        }
        else if (!line.compare(0, 7, "usemtl "))
        {
            //usemtl xxx， 读取xxx作为材质名称
            std::string filename = line.substr(7);
			submesh_name.push_back(filename);
            submesh_idx.push_back(facet_vrt.size() / 3);
        }
        else if (!line.compare(0, 2, "f ")) {
            int f, t, n;
            iss >> trash;
            int cnt = 0;
            while (iss >> f >> trash >> t >> trash >> n) {
                facet_vrt.push_back(--f);
                facet_tex.push_back(--t);
                facet_nrm.push_back(--n);
                cnt++;
            }
            if (3 != cnt) {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }
    std::cerr << "# v# " << nverts() << " f# " << nfaces() << " vt# " << tex_coord.size() << " vn# " << norms.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap);
}

int Model::nverts() const {
    return verts.size();
}

int Model::nfaces() const {
    return facet_vrt.size() / 3;
}

vec3 Model::vert(const int i) const {
    return verts[i];
}

vec3 Model::vert(const int iface, const int nthvert) const {
    return verts[facet_vrt[iface * 3 + nthvert]];
}

vec2 Model::uv(const int iface, const int nthvert) const {
    return tex_coord[facet_tex[iface * 3 + nthvert]];
}

vec3 Model::normal(const int iface, const int nthvert) const {
    return norms[facet_nrm[iface * 3 + nthvert]];
}

void Model::load_texture(std::string filename, const std::string suffix, std::vector<TGAImage>& img) {
    if (submesh_idx.size() == 0) //没有submesh，此时直接读取默认的一张纹理作为采样的纹理
    {
        TGAImage tmp;
        std::string texfile = "obj/textures/" + modelname + suffix;
        if (tmp.read_tga_file(texfile.c_str()))
        {
			std::cout << "load texture file " << texfile << " success" << std::endl;
			img.push_back(tmp);
		}
        else
        {
			std::cerr << "load texture file " << texfile << " failed" << std::endl;
		}
        return;
    }

    for (int i = 0; i < submesh_name.size(); i++) //有sumesh，此时读取每个submesh对应的纹理，这种情况对应比如知更鸟，多个submesh对应多张贴图
    {
        TGAImage tmp;
        size_t dot = filename.find_last_of(".");
        if (dot == std::string::npos) return;
		std::string texfile = "obj/textures/" + submesh_name[i] + suffix; //_diffuse.tga
        //texfile = "obj/textures/RobinYi_diffuse.tga";
        
        if (tmp.read_tga_file(texfile.c_str()))
        {
            std::cout << "load texture file " << texfile << " success" << std::endl;
            img.push_back(tmp);
        }
	}
}

const TGAImage& Model::diffuse(int faceIndex) const
{
    //在对应的区间内，则返回对应的纹理
    for (int i = 0; i < submesh_idx.size(); i++)
    {
        if (faceIndex >= submesh_idx[i])  //0, 400, 1500, map0, map1, map2
        {
            if (i == submesh_idx.size() - 1)
            {
                //std::cout << "faceIndex :: " << faceIndex << "tmpIndex  " << tmpIndex << std::endl;
				return diffusemap[i];
			}
            else if (faceIndex < submesh_idx[i + 1])
            {
                //std::cout << "faceIndex!!! :: " << faceIndex << "tmpIndex  " << tmpIndex << std::endl;
                return diffusemap[i];
            }
                
		}
	}
    std::cout << "faceIndex :: " << faceIndex << std::endl;
	return diffusemap[0]; //todo:现在默认返回第一个纹理，后续要做更好的逻辑判断
}




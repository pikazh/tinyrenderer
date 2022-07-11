#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

#include "model.h"

Model::Model(const char *filename) : verts_(), faces_(), textures_coords_(), textures_()
{
    std::ifstream in;
    in.open(filename, std::ifstream::in);
    if (in.fail())
        return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++)
                iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            std::vector<int> tex;
            int itrash, idx, texture_coord;
            iss >> trash;
            while (iss >> idx >> trash >> texture_coord >> trash >> itrash)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
                tex.push_back(texture_coord-1);
            }
            faces_.push_back(f);
            textures_.push_back(tex);
        }
        else if (!line.compare(0, 3, "vt "))
        {
            iss >> trash;
            iss >> trash;
            Vec2f v;
            for (int i = 0; i < 2; i++)
                iss >> v.raw[i];

            textures_coords_.push_back(v);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << " t# " << textures_coords_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nverts()
{
    return (int)verts_.size();
}

int Model::nfaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}

std::vector<int> Model::texture(int idx)
{
    return textures_[idx];
}

Vec2f Model::texture_coords(int i)
{
    return textures_coords_[i];
}

#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>

#include "geometry.h"

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int>> faces_;

    std::vector<Vec2f> textures_coords_;
    std::vector<std::vector<int>> textures_;

public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    std::vector<int> face(int idx);
    std::vector<int> texture(int idx);
    Vec2f texture_coords(int i);
};

#endif //__MODEL_H__

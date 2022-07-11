#include <cmath>
#include <iostream>

#include "tgaimage.h"
#include "model.h"

inline Vec3f barycentric(Vec2i *tri, Vec2i p)
{
    Vec3f u = Vec3f(tri[2].x - tri[0].x, tri[1].x - tri[0].x, tri[0].x - p.x) ^ Vec3f(tri[2].y - tri[0].y, tri[1].y - tri[0].y, tri[0].y - p.y);

    if (std::abs(u.z) < 1.f)
        return Vec3f(-1.f, -1.f, -1.f);

    u = u / u.z;
    if (u.x < 0.f || u.y < 0.f || (u.x + u.y) > 1.f)
        return Vec3f(-1.f, -1.f, -1.f);

    return Vec3f(1.f - u.y - u.x, u.y, u.x);
}

void triangle_using_barycentric_coordinates(Vec3f t0, Vec3f t1, Vec3f t2, float *zbuffer, TGAImage &image, TGAImage &texture, Vec2f *texture_coords, float intensity)
{
    Vec2i bbox_min(image.width() - 1, image.height() - 1);
    Vec2i bbox_max(0, 0);
    Vec3f vertices[3] = {t0, t1, t2};
    Vec2i vertices_i[3] = {Vec2i(t0.x, t0.y), Vec2i(t1.x, t1.y), Vec2i(t2.x, t2.y)};

    for (int i = 0; i < 3; ++i)
    {
        bbox_min.x = std::max(0, std::min(bbox_min.x, vertices_i[i].x));
        bbox_min.y = std::max(0, std::min(bbox_min.y, vertices_i[i].y));

        bbox_max.x = std::min(image.width() - 1, std::max(bbox_max.x, vertices_i[i].x));
        bbox_max.y = std::min(image.height() - 1, std::max(bbox_max.y, vertices_i[i].y));
    }

    for (int x = bbox_min.x; x <= bbox_max.x; ++x)
    {
        for (int y = bbox_min.y; y < bbox_max.y; ++y)
        {
            Vec3f b = barycentric(vertices_i, Vec2i(x, y));
            if (b.x == -1.f)
            {
                continue;
            }
            float z = vertices[0].z * b.x + vertices[1].z * b.y + vertices[2].z * b.z;
            int z_buffer_index = x * image.height() + y;
            if (zbuffer[z_buffer_index] < z)
            {
                zbuffer[z_buffer_index] = z;

                // to get the texture coordinates of this interpolated point, multiply its barycentric coordinates by the vertices of the texture triangle
                Vec2f tex_coord = texture_coords[0] * b.x + texture_coords[1] * b.y + texture_coords[2] * b.z;
                TGAColor color = texture.color_at(int(tex_coord.x), int(tex_coord.y));
                color[0] *= intensity;
                color[1] *= intensity;
                color[2] *= intensity;
                image.set_color_at(x, y, color);
            }
        }
    }
}

int main(int /*argc*/, char ** /*argv*/)
{
    TGAImage texture(1024, 1024, TGAImage::Format::RGB);
    if (!texture.read_tga_file("obj/african_head_diffuse.tga"))
    {
        return 1;
    }

    auto model = new Model("obj/african_head.obj");

    const int screen_width = 800;
    const int screen_height = 800;
    TGAImage screen(screen_width, screen_height, TGAImage::Format::RGB);
    float zbuffer[screen_width * screen_height];
    for (int i = 0; i < screen_width * screen_height; i++)
        zbuffer[i] = -10000.f;

    Vec3f light_dir(0, 0, -1);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        std::vector<int> tex = model->texture(i);
        Vec3f screen_coords[3];
        Vec3f world_coords[3];
        Vec2f texture_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec3f((v.x + 1.) * screen_width / 2., (v.y + 1.) * screen_height / 2., v.z);
            world_coords[j] = v;

            texture_coords[j] = model->texture_coords(tex[j]);
            texture_coords[j].u *= texture.width();
            texture_coords[j].v *= texture.height();
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0)
        {
            triangle_using_barycentric_coordinates(screen_coords[0], screen_coords[1], screen_coords[2], zbuffer, screen, texture, texture_coords, intensity);
        }
    }

    screen.write_tga_file("output.tga");
    delete model;

    return 0;
}

#include <cmath>
#include <vector>

#include "tgaimage.h"
#include "model.h"

void triangle_by_line_sweeping(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if (t0.y > t1.y)
    {
        std::swap(t0, t1);
    }
    if (t0.y > t2.y)
    {
        std::swap(t0, t2);
    }
    if (t1.y > t2.y)
    {
        std::swap(t1, t2);
    }

    int total_height = t2.y - t0.y;
    if (total_height == 0)
        return;

    int segment_height_lower = t1.y - t0.y;
    if (segment_height_lower != 0)
    {
        Vec2i v1 = t1 - t0;
        Vec2i v2 = t2 - t0;
        for (int cur_y = t0.y, current_height = 0; cur_y <= t1.y; cur_y++, current_height++)
        {
            int xl = t0.x + int(std::round(float(current_height) / segment_height_lower * v1.x));
            int xr = t0.x + int(std::round(float(current_height) / total_height * v2.x));
            if (xl > xr)
                std::swap(xl, xr);

            for (int i = xl; i <= xr; i++)
            {
                image.set_color_at(i, cur_y, color);
            }
        }
    }

    int segment_height_upper = t2.y - t1.y;
    if (segment_height_upper != 0)
    {
        Vec2i v1 = t2 - t1;
        Vec2i v2 = t2 - t0;
        for (int cur_y = t1.y, current_height = 0; cur_y <= t2.y; cur_y++, current_height++)
        {
            int xl = t1.x + int(std::round(float(current_height) / segment_height_upper * v1.x));
            int xr = t0.x + int(std::round(float(current_height + segment_height_lower) / total_height * v2.x));
            if (xl > xr)
                std::swap(xl, xr);

            for (int i = xl; i <= xr; i++)
            {
                image.set_color_at(i, cur_y, color);
            }
        }
    }
}

inline bool inside_test(Vec2i *tri, Vec2i p)
{
    Vec3f u = Vec3f(tri[2].x - tri[0].x, tri[1].x - tri[0].x, tri[0].x - p.x) ^ Vec3f(tri[2].y - tri[0].y, tri[1].y - tri[0].y, tri[0].y - p.y);

    if (std::abs(u.z) < 1)
        return false;

    u = u / u.z;
    if (u.x < 0.f || u.y < 0.f || (u.x + u.y) > 1.f)
        return false;

    return true;
}

void triangle_using_barycentric_coordinates(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    Vec2i bbox_min(image.width() - 1, image.height() - 1);
    Vec2i bbox_max(0, 0);
    std::vector<Vec2i> vertices;
    vertices.push_back(t0);
    vertices.push_back(t1);
    vertices.push_back(t2);

    for (auto it = vertices.begin(); it != vertices.end(); ++it)
    {
        bbox_min.x = std::max(0, std::min(bbox_min.x, it->x));
        bbox_min.y = std::max(0, std::min(bbox_min.y, it->y));

        bbox_max.x = std::min(image.width()-1, std::max(bbox_max.x, it->x));
        bbox_max.y = std::min(image.height()-1, std::max(bbox_max.y, it->y));
    }

    for (int x = bbox_min.x; x <= bbox_max.x; ++x)
    {
        for (int y = bbox_min.y; y < bbox_max.y; ++y)
        {
            if (inside_test(vertices.data(), Vec2i(x, y)))
            {
                image.set_color_at(x, y, color);
            }
        }
    }
}

int main(int /*argc*/, char ** /*argv*/)
{
    /*
    const TGAColor red = TGAColor(255, 0, 0, 255);
    TGAImage image(100, 100, TGAImage::Format::RGB);
    triangle_by_line_sweeping(Vec2i(50, 20), Vec2i(20, 70), Vec2i(80, 60), image, red);
    image.write_tga_file("output.tga");
    */
    /*
     const TGAColor red = TGAColor(255, 0, 0, 255);
     TGAImage image(100, 100, TGAImage::Format::RGB);
     triangle_using_barycentric_coordinates(Vec2i(91, 20), Vec2i(20, 70), Vec2i(80, 90), image, red);
     image.write_tga_file("output.tga");
     */

    Vec3f light_dir(0, 0, -1); // define light_dir
    auto model = new Model("obj/african_head.obj");

    const int width = 800;
    const int height = 800;
    TGAImage image(width, height, TGAImage::Format::RGB);

    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for (int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x + 1.) * width / 2., (v.y + 1.) * height / 2.);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        n.normalize();
        float intensity = n * light_dir;
        if (intensity > 0)
        {
            triangle_using_barycentric_coordinates(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity * 255, intensity * 255, 0, 255));
        }
    }
    
    image.write_tga_file("output.tga");
    delete model;

    return 0;
}

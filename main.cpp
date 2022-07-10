#include <cmath>

#include "tgaimage.h"
#include "model.h"

void bresenham_line_drawing(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x1 - x0) < std::abs(y1 - y0))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);

        steep = true;
    }

    if (x1 < x0)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = std::abs(y1 - y0);

    int fraction = 2 * dy - dx;
    int y = y0;
    int direction_y = y1 < y0 ? -1 : 1;

    if (steep)
    {
        for (int x = x0; x <= x1; x++)
        {
            image.set_color_at(y, x, color);

            if (fraction > 0)
            {
                y += direction_y;
                fraction -= 2 * dx;
            }

            fraction += 2 * dy;
        }
    }
    else
    {
        for (int x = x0; x <= x1; x++)
        {
            image.set_color_at(x, y, color);

            if (fraction > 0)
            {
                y += direction_y;
                fraction -= 2 * dx;
            }

            fraction += 2 * dy;
        }
    }
}

int main(int /*argc*/, char ** /*argv*/)
{
    /*    const TGAColor white = TGAColor(255, 255, 255, 255);
        const TGAColor red = TGAColor(255, 0, 0, 255);
        TGAImage image(100, 100, TGAImage::RGB);
        bresenham_line_drawing(13, 20, 80, 40, image, white);
        bresenham_line_drawing(20, 13, 40, 80, image, red);
        bresenham_line_drawing(80, 40, 13, 20, image, red);
        bresenham_line_drawing(40, 80, 20, 13, image, white);
        bresenham_line_drawing(80, 10, 13, 90, image, red);
        bresenham_line_drawing(13, 90, 80, 10, image, white);

        image.write_tga_file("output.tga");
    */

    auto model = new Model("obj/african_head.obj");

    const int width = 800;
    const int height = 800;
    const TGAColor white = TGAColor(255, 255, 255, 255);

    TGAImage image(width, height, TGAImage::Format::RGB);
    for (int i = 0; i < model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j + 1) % 3]);
            int x0 = (v0.x + 1.) * width / 2.;
            int y0 = (v0.y + 1.) * height / 2.;
            int x1 = (v1.x + 1.) * width / 2.;
            int y1 = (v1.y + 1.) * height / 2.;
            bresenham_line_drawing(x0, y0, x1, y1, image, white);
        }
    }

    image.write_tga_file("output.tga");
    delete model;
    return 0;
}

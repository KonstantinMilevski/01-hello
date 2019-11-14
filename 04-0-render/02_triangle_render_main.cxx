#include "02_triangle_render.hxx"

int main()
{
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };

    canvas image;

    triangle_render render_tri(image, width, height);
    render_tri.clear(black);

    std::vector<position> triangle;
    triangle.push_back(position{ 0, 0 });
    triangle.push_back(position{ width - 1, height - 1 });
    triangle.push_back(position{ 0, height - 1 });

    render_tri.draw_triangle(triangle, 3, green);

    image.save_image("02_triangle.ppm");

    render_tri.clear(black);

    size_t max_x = 10;
    size_t max_y = 10;

    std::vector<position> triangles;

    for (size_t i = 0; i < max_x; ++i)
    {
        for (size_t j = 0; j < max_y; ++j)
        {
            int32_t step_x = (width - 1) / max_x;
            int32_t step_y = (height - 1) / max_y;

            position v0{ 0 + static_cast<int>(i) * step_x,
                         0 + static_cast<int>(j) * step_y };
            position v1{ v0.x + step_x, v0.y + step_y };
            position v2{ v0.x, v0.y + step_y };
            position v3{ v0.x + step_x, v0.y };

            triangles.push_back(v0);
            triangles.push_back(v1);
            triangles.push_back(v2);

            triangles.push_back(v0);
            triangles.push_back(v3);
            triangles.push_back(v1);
        }
    }

    render_tri.draw_triangle(triangles, triangles.size(), green);

    image.save_image("03_triangles.ppm");

    //    const int dx = 100;
    //    const int dy = 100;
    //    render.clear(black);
    //    std::vector<position> triangle1;
    //    int                   i = 1;
    //    //    for (int i = 0; i < width; i += dx)
    //    //    {
    //    for (int j = 0; j < height; j += dy)
    //    {
    //        triangle1.push_back(position{ i, j });
    //        triangle1.push_back(position{ i + 50, j + 50 });
    //        triangle1.push_back(position{ i, j + 50 });
    //    }
    //    //  }
    //    std::cout << triangle1.size() << std::endl;
    //    render.draw_triangle(triangle1, triangle1.size(), green);
    // image.save_image("021_triangle.ppm");

    return 0;
}

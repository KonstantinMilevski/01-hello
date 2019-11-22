#include "line_triangle_render.hxx"

int main()
{

    const color          black = { 0, 0, 0 };
    const color          green = { 0, 255, 0 };
    const color          blue  = { 0, 0, 255 };
    canvas               image;
    line_triangle_render line_triangle(image, width, height);

    line_triangle.clear(black);
    line_triangle.draw_line_triangle(position{ 50, 50 },
                                     position{ width - 50, height - 50 },
                                     position{ 100, 200 }, green);
    std::vector<position> triangle;
    triangle.push_back(position{ 0, 0 });
    triangle.push_back(position{ width - 1, height - 1 });
    triangle.push_back(position{ 0, height - 1 });

    line_triangle.draw_line_triangle(triangle, 3, blue);
    size_t                max_x = 10;
    size_t                max_y = 10;
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

    line_triangle.draw_line_triangle(triangles, triangles.size(), green);

    std::string file_name = "03.ppm ";
    image.save_image(file_name);
    return 0;
}

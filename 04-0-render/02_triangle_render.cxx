#include "02_triangle_render.hxx"

triangle_render::triangle_render(canvas& buffer, size_t width, size_t height)
    : line_render(buffer, width, height)
{
}
// using pixels = std::vector<position>;
pixels triangle_render::pixels_positions_triangle(const position& v1,
                                                  const position& v2,
                                                  const position& v3)
{
    using namespace std;
    pixels points_on_side;
    //    pixels pixels_pos;

    //    for (auto [start, end] : { pair{ v0, v1 }, pair{ v1, v2 }, pair{ v2,
    //    v0 } })
    //    {
    //        for (auto pos : line_render::pixels_positions(start, end))
    //        {
    //            pixels_pos.push_back(pos);
    //        }
    //    }
    for (auto point : line_render::pixels_positions(v1, v2))
        points_on_side.push_back(point);
    for (auto point : line_render::pixels_positions(v1, v3))
        points_on_side.push_back(point);
    for (auto point : line_render::pixels_positions(v2, v3))
        points_on_side.push_back(point);

    return points_on_side;
}
void triangle_render::draw_empty_triangle(const std::vector<position>& points,
                                          const size_t& num, const color& col)
{
    pixels triangles_edge_pixels;
    for (int i = 0; i < num; i += 3)
    {
        position v1 = points.at(i + 0);
        position v2 = points.at(i + 1);
        position v3 = points.at(i + 2);

        for (auto pixel_pos : pixels_positions_triangle(v1, v2, v3))
        {
            triangles_edge_pixels.push_back(pixel_pos);
        }
    }

    for (auto c : triangles_edge_pixels)
        set_pixel(c, col);
}

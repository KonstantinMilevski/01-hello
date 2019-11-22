#include "line_triangle_render.hxx"

struct indexed_triangle_render : line_triangle_render
{
    indexed_triangle_render(canvas& buffer, size_t width, size_t height);
    void draw_indexed_triangle(const std::vector<position>& vertex,
                               const std::vector<uint8_t>&  indexes,
                               const color&                 c);
};

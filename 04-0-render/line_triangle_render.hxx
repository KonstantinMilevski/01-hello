#include "line_render.hxx"

struct line_triangle_render : line_render
{
    line_triangle_render(canvas& buffer, size_t width, size_t height);

    void draw_line_triangle(const position& top1, const position& top2,
                            const position& top3, const color& c);
    void draw_line_triangle(const pixels& tops, const size_t& num,
                            const color& c);
};

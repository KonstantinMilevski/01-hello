#include "line_triangle_render.hxx"

line_triangle_render::line_triangle_render(canvas& buffer, size_t width,
                                           size_t height)
    : line_render(buffer, width, height)
{
}

void line_triangle_render::draw_line_triangle(const position& top1,
                                              const position& top2,
                                              const position& top3,
                                              const color&    c)
{
    using namespace std;
    draw_line(top1, top2, c);
    draw_line(top3, top2, c);
    draw_line(top1, top3, c);
}

void line_triangle_render::draw_line_triangle(const pixels& tops,
                                              const size_t& num, const color& c)
{
    for (size_t i = 0; i < num; i += 3)
    {
        draw_line_triangle(tops[i], tops[i + 1], tops[i + 2], c);
    }
}

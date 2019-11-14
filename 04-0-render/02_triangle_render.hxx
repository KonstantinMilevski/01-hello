#pragma once
#include "01_line_render.hxx"

struct triangle_render : line_render
{
    triangle_render(canvas&, size_t, size_t);
    virtual pixels pixels_positions_triangle(const position&, const position&,
                                             const position&);
    void           draw_triangle(const std::vector<position>&, const size_t&,
                                 const color&);
};

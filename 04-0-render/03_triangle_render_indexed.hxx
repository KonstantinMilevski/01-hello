#pragma once
#include "02_triangle_render.hxx"

struct triangle_indexed_render : triangle_render
{
    triangle_indexed_render(canvas&, size_t, size_t);

    void draw_triangle(std::vector<position>& vertexes,
                       std::vector<uint8_t>& indexes, color c);
};

#include "indexed_triangle_render.hxx"

indexed_triangle_render::indexed_triangle_render(canvas& buffer, size_t width,
                                                 size_t height)
    : line_triangle_render(buffer, width, height)
{
}

void indexed_triangle_render::draw_indexed_triangle(
    const std::vector<position>& vertex, const std::vector<uint8_t>& indexes,
    const color& c)
{
    for (size_t i = 0; i < indexes.size(); i += 3)
    {
        uint8_t index0 = indexes.at(i);
        uint8_t index1 = indexes.at(i + 1);
        uint8_t index2 = indexes.at(i + 2);

        position top0 = vertex[index0];
        position top1 = vertex[index1];
        position top2 = vertex[index2];

        draw_line_triangle(top0, top1, top2, c);
    }
}

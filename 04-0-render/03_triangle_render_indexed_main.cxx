#include "03_triangle_render_indexed.hxx"

int main(int argc, char* argv[])
{
    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };

    canvas image;

    std::vector<position> triangles_for_index;

    size_t max_x = 2;
    size_t max_y = 2;

    int32_t step_x = (width - 1) / max_x;
    int32_t step_y = (height - 1) / max_y;

    for (size_t i = 0; i <= max_y; ++i)
    {
        for (size_t j = 0; j <= max_x; ++j)
        {
            position v{ static_cast<int>(j) * step_x,
                        static_cast<int>(i) * step_y };

            triangles_for_index.push_back(v);
        }
    }

    assert(triangles_for_index.size() == (max_x + 1) * (max_y + 1));

    std::vector<uint8_t> indexes;

    for (size_t x = 0; x < max_x; ++x)
    {
        for (size_t y = 0; y < max_y; ++y)
        {
            uint8_t index0 = static_cast<uint8_t>(y * (max_y + 1) + x);
            uint8_t index1 = static_cast<uint8_t>(index0 + (max_y + 1) + 1);
            uint8_t index2 = index1 - 1;
            uint8_t index3 = index0 + 1;

            indexes.push_back(index0);
            indexes.push_back(index1);
            indexes.push_back(index2);

            indexes.push_back(index0);
            indexes.push_back(index3);
            indexes.push_back(index1);
        }
    }

    triangle_indexed_render indexed_render(image, width, height);
    indexed_render.clear(black);

    indexed_render.draw_empty_triangle(triangles_for_index, indexes, green);

    image.save_image("04_triangles_indexes.ppm");

    canvas tex_image;
    tex_image.load_image("04_triangles_indexes.ppm");

    assert(image == tex_image);

    return 0;
}

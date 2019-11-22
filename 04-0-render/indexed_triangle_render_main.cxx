#include "indexed_triangle_render.hxx"

int main()
{

    const color black = { 0, 0, 0 };
    const color green = { 0, 255, 0 };
    const color blue  = { 0, 0, 255 };
    canvas      image;

    indexed_triangle_render indexed_triangle(image, width, height);
    indexed_triangle.clear(black);
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

    for (size_t y = 0; y < max_y; ++y)
    {
        for (size_t x = 0; x < max_x; ++x)
        {
            uint8_t index0 = static_cast<uint8_t>(x + (max_x + 1) * y);
            uint8_t index1 = static_cast<uint8_t>(index0 + 1);
            uint8_t index2 = index1;
            indexes.push_back(index0);
            indexes.push_back(index1);
            indexes.push_back(index2);
            if (y == (max_y - 1))
            {
                uint8_t index0 =
                    static_cast<uint8_t>(x + (max_x + 1) * (y + 1));
                uint8_t index1 = static_cast<uint8_t>(index0 + 1);
                uint8_t index2 = index1;
                indexes.push_back(index0);
                indexes.push_back(index1);
                indexes.push_back(index2);
            }
        }
    }

    for (size_t x = 0; x <= max_x; ++x)
    {
        for (size_t y = 0; y < max_y; ++y)
        {
            uint8_t index0 = static_cast<uint8_t>((max_x + 1) * y + x);
            uint8_t index3 = static_cast<uint8_t>(index0 + (max_x + 1));
            uint8_t index4 = index3;

            indexes.push_back(index0);
            indexes.push_back(index3);
            indexes.push_back(index4);
        }
    }

    indexed_triangle.draw_indexed_triangle(triangles_for_index, indexes, green);

    std::string file_name = "04.ppm";
    image.save_image(file_name);
}

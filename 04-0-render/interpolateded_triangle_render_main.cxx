#include "interpolateded_triangle_render.hxx"

int main()
{

    const color  black = { 0, 0, 0 };
    const color  green = { 0, 255, 0 };
    const color  blue  = { 0, 0, 255 };
    canvas       image;
    canvas       image_tri;
    const size_t max_x = 17;
    const size_t max_y = 19;

    struct program : gfx_program
    {
        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;

            //            // rotate
            //            double alpha = 3.14159 / 6; // 30 degree
            //            double x     = out.f0;
            //            double y     = out.f1;
            //            out.f0       = x * std::cos(alpha) - y *
            //            std::sin(alpha); out.f1       = x * std::sin(alpha) +
            //            y * std::cos(alpha);

            //            // scale into 3 times
            //            out.f0 *= 0.3;
            //            out.f1 *= 0.3;

            //            // move
            //            out.f0 += (width / 2);
            //            out.f1 += (height / 2);

            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;
            //            out.r = static_cast<uint8_t>(v_in.f2 * 255);
            //            out.g = static_cast<uint8_t>(v_in.f3 * 255);
            //            out.b = static_cast<uint8_t>(v_in.f4 * 255);
            return out;
        }
    } program01;

    interpolateded_triangle_render interpolated_cell(image, width, height);

    interpolated_cell.clear(black);

    interpolated_cell.set_gfx_program(program01);

    std::vector<vertex>  triangles_for_index;
    std::vector<uint8_t> indexes;
    draw_cells(max_x, max_y, triangles_for_index, indexes);

    interpolated_cell.draw_interpolated_triangle(triangles_for_index, indexes);

    image.save_image("05_sell.ppm");

    interpolateded_triangle_render interpolated_triangle(image_tri, width,
                                                         height);
    interpolated_triangle.clear(black);
    interpolated_triangle.set_gfx_program(program01);
    std::vector<vertex>  triangle_v{ { 50, 0, 1, 0, 0, 0, 0, 0 },
                                    { 0, 150, 0, 1, 0, 0, 239, 0 },
                                    { 200, 100, 0, 0, 1, 319, 239, 0 } };
    std::vector<uint8_t> indexes_v{ 0, 1, 2 };

    interpolated_triangle.draw_interpolated_triangle(triangle_v, indexes_v);
    image_tri.save_image("05_tri.ppm");

    return 0;
}

#include "04_triangle_render_interpolated.hxx"

// void draw_grid(const size_t& w, const size_t& h)
//{
//    size_t max_x = w;
//    size_t max_y = h;

//    int32_t step_x = (width - 1) / max_x;
//    int32_t step_y = (height - 1) / max_y;

//    for (size_t i = 0; i <= max_y; ++i)
//    {
//        for (size_t j = 0; j <= max_x; ++j)
//        {
//            position v{ static_cast<int>(j) * step_x,
//                        static_cast<int>(i) * step_y };

//            triangles_for_index.push_back(v);
//        }
//    }

//    assert(triangles_for_index.size() == (max_x + 1) * (max_y + 1));

//    std::vector<uint8_t> indexes;

//    for (size_t x = 0; x < max_x; ++x)
//    {
//        for (size_t y = 0; y < max_y; ++y)
//        {
//            uint8_t index0 = static_cast<uint8_t>(y * (max_y + 1) + x);
//            uint8_t index1 = static_cast<uint8_t>(index0 + (max_y + 1) + 1);
//            uint8_t index2 = index1 - 1;
//            uint8_t index3 = index0 + 1;

//            indexes.push_back(index0);
//            indexes.push_back(index1);
//            indexes.push_back(index2);

//            indexes.push_back(index0);
//            indexes.push_back(index3);
//            indexes.push_back(index1);
//        }
//    }
//}
int main(int argc, char* argv[])
{

    const color black = { 0, 0, 0 };
    const color white = { 255, 255, 255 };
    const color green = { 0, 255, 0 };
    canvas      image;

    triangle_interpolated interpolated_render(image, width, height);

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
            out.r = static_cast<uint8_t>(v_in.f2 * 255);
            out.g = static_cast<uint8_t>(v_in.f3 * 255);
            out.b = static_cast<uint8_t>(v_in.f4 * 255);
            return out;
        }
    } program01;

    interpolated_render.clear(black);
    interpolated_render.set_gfx_program(program01);
    // clang-format off


     //                                 x    y   r  g  b   u    v   &
//                                     f0   f1  f2 f3 f4  f5   f6
    std::vector<vertex> triangle_v{ {   50,   50,  1, 1, 1,  0,   0,  0 },
                                    {   50, 189,  1, 1, 1,  0,  239, 0 },
                                    { 200, 189,  1, 1, 1, 319, 239, 0 } };
std::vector<uint8_t> indexes_v {0,1,2};
    // clang-format on

    std::vector<vertex>  triangle_l;
    std::vector<uint8_t> indexes_l;

    size_t max_x = 8;
    size_t max_y = 8;

    int32_t step_x = (width - 1) / max_x;
    int32_t step_y = (height - 1) / max_y;

    for (size_t x = 0; x <= width; x = x + step_x)
    {
        vertex v0{ x, 0, 0, 0, 1, 0, 0, 0 };
        vertex v1{ x, (height - 1), 1, 0, 0, 0, 0, 0 };
        interpolated_render.build_line(v0, v1);
        //  interpolated_render.print_line(v0, v1);
    }
    for (size_t y = 0; y <= height; y = y + step_y)
    {
        vertex v0{ 0, y, 0, 0, 1, 0, 0, 0 };
        vertex v1{ (width - 1), y, 0, 0, 1, 0, 0, 0 };
        interpolated_render.build_line(v0, v1);
        // interpolated_render.print_line(v0, v1);
    }
    interpolated_render.draw_empty_tri_({ 50, 50, 0, 1, 0, 0, 0, 0 },
                                        { 180, 90, 0, 0, 0, 0, 0, 0 },
                                        { 50, 200, 0, 1, 0, 0, 0, 0 });
    image.save_image("05_interpolated.ppm");

    // texture example
    struct program_tex : gfx_program
    {
        std::array<color, buffer_size> texture;

        void   set_uniforms(const uniforms&) override {}
        vertex vertex_shader(const vertex& v_in) override
        {
            vertex out = v_in;
            /*
                        // rotate
                        double alpha = 3.14159 / 6; // 30 degree
                        double x     = out.f0;
                        double y     = out.f1;
                        out.f0       = x * std::cos(alpha) - y *
               std::sin(alpha); out.f1       = x * std::sin(alpha) + y
               * std::cos(alpha);
            */
            /*
            // scale into 3 times
            out.f0 *= 0.8;
            out.f1 *= 0.8;
            */
            /*
                        // move
                        out.f0 += (width / 2);
                        out.f1 += (height / 2);
            */
            return out;
        }
        color fragment_shader(const vertex& v_in) override
        {
            color out;

            out.r = static_cast<uint8_t>(v_in.f2 * 255);
            out.g = static_cast<uint8_t>(v_in.f3 * 255);
            out.b = static_cast<uint8_t>(v_in.f4 * 255);

            color from_texture = sample2d(v_in.f5, v_in.f6);
            out.r += from_texture.r;
            out.g = from_texture.g;
            out.b += from_texture.b;
            return out;
        }

        void set_texture(const std::array<color, buffer_size>& tex)
        {
            texture = tex;
        }

        color sample2d(double u_, double v_)
        {
            uint32_t u = static_cast<uint32_t>(std::round(u_));
            uint32_t v = static_cast<uint32_t>(std::round(v_));

            color c = texture.at(v * width + u);
            return c;
        }
    };

    program_tex program02;
    program02.set_texture(image);

    interpolated_render.set_gfx_program(program02);

    interpolated_render.clear(black);

    interpolated_render.draw_filled_triangle(triangle_v, indexes_v);

    image.save_image("06_textured_triangle.ppm");

    return 0;
}

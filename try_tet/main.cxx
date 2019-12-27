#include "engine.hxx"
#include "texture_gl_es20.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

// const int m          = 10;
// const int n          = 10;
// int       fild[m][n] = { 0 };
//// enum class fig_type
//{
//    type_I,
//    type_S,
//    type_Z,
//    type_T,
//    type_L,
//    type_J,
//    type_O
//};

int                              n             = 20;
static const std::vector<vertex> figures_coord = {
    { 0.0, 1.5, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 },
    { 0.0, -0.5, 0.0, 0.0 },
    { 0.0, -1.5, 0.0, 0.0 }, // I
    //
    { 0.5, 1.0, 0.0, 0.0 },
    { 0.5, 0.0, 0.0, 0.0 },
    { -0.5, 0.0, 0.0, 0.0 },
    { -0.5, -1.0, 0.0, 0.0 }, // Z

    { 0.5, -1.5, 0.0, 0.0 },
    { 0.5, -0.5, 0.0, 0.0 },
    { -0.5, -0.5, 0.0, 0.0 },
    { -0.5, 0.5, 0.0, 0.0 }, // S ??

    { -0.5, -1.0, 0.0, 0.0 },
    { -0.5, 0.0, 0.0, 0.0 },
    { -0.5, 1.0, 0.0, 0.0 },
    { 0.5, 0.0, 0.0, 0.0 }, // T

    { -0.5, 1.0, 0.0, 0.0 },
    { -0.5, 0.0, 0.0, 0.0 },
    { -0.5, -1.0, 0.0, 0.0 },
    { 0.5, -1.0, 0.0, 0.0 }, // L

    { 0.5, 1.0, 0.0, 0.0 },
    { 0.5, 0.0, 0.0, 0.0 },
    { 0.5, -1.0, 0.0, 0.0 },
    { -0.5, -1.0, 0.0, 0.0 }, // J

    { -0.5, 0.5, 0.0, 0.0 },
    { -0.5, -0.5, 0.0, 0.0 },
    { 0.5, 0.5, 0.0, 0.0 },
    { 0.5, -0.5, 0.0, 0.0 } // O
};

/// global
float        first_pos;
static float window_scale = static_cast<float>(screen_height) / screen_width;
// tri2 transform_pixel_coord_to_GL(size_t tex_w, size_t tex_h, tri2& t)
//{
//    tri2 n;
//    n.v[0].pos.x = t.v[0].pos.x * 2 / width - 1.0f;
//    n.v[0].pos.y = t.v[0].pos.y * 2 / heigh - 1.0f;
//    n.v[1].pos.x = t.v[1].pos.x * 2 / width - 1.0f;
//    n.v[1].pos.y = t.v[1].pos.y * 2 / heigh - 1.0f;
//    n.v[2].pos.x = t.v[2].pos.x * 2 / width - 1.0f;
//    n.v[2].pos.y = t.v[2].pos.y * 2 / heigh - 1.0f;

//    n.v[0].uv.x = t.v[0].uv.x * 2 / width - 1.0f;
//    n.v[0].uv.y = t.v[0].uv.y * 2 / heigh - 1.0f;
//    n.v[1].uv.x = t.v[1].uv.x * 2 / width - 1.0f;
//    n.v[1].uv.y = t.v[1].uv.y * 2 / heigh - 1.0f;
//    n.v[2].uv.x = t.v[2].uv.x * 2 / width - 1.0f;
//    n.v[2].uv.y = t.v[2].uv.y * 2 / heigh - 1.0f;
//    return n;
//}
// void check_border(float& x)
//{
//    float dx = quad_size / static_cast<float>(screen_width);
//    if (x < -1 + dx)
//        x = -1 + dx;
//    else if (x > 1.f - 3 * dx)
//        x = 1.f - 3 * dx;
//    else
//        return;
//}
struct rect
{
    rect(vec2 p, vec2 s)
        : pos{ p }
        , size{ s }
    {
    }
    vec2 pos;
    vec2 size;
};
struct block
{
    block() = default;

    block(vertex pos, /*float size,*/ rect uv_pos, texture* tex)
        : centr_pos_(pos)
        //, size_(size)
        , rect_pos_(uv_pos)
        , tex_(tex)
    {
    }
    std::vector<tri2> build_block()
    {
        float                 size_ = 1.f;
        std::array<vertex, 4> quad;
        // you can change colore of texture with shift uv coord
        ///   0            1
        ///   *------------*
        ///   |           /|
        ///   |         /  |
        ///   |      P/    |  // P - pos_ center - OpenGL (0,0)
        ///   |     /      |  // vertex 3 - texture (0,0)
        ///   |   /        |
        ///   | /          |
        ///   *------------*
        ///   3            2
        ///
        /// 0 left-up, clockwise
        /// vec2.pos
        quad[0].pos.x = centr_pos_.pos.x - size_ / 2;
        quad[0].pos.y = centr_pos_.pos.y + size_ / 2;
        quad[1].pos.x = centr_pos_.pos.x + size_ / 2;
        quad[1].pos.y = centr_pos_.pos.y + size_ / 2;
        quad[2].pos.x = centr_pos_.pos.x + size_ / 2;
        quad[2].pos.y = centr_pos_.pos.y - size_ / 2;
        quad[3].pos.x = centr_pos_.pos.x - size_ / 2;
        quad[3].pos.y = centr_pos_.pos.y - size_ / 2;
        /// vec2.uv, OpenGL texture lower left angle is (0, 0) coordinate
        quad[3].uv.x = rect_pos_.pos.x + centr_pos_.uv.x;
        quad[3].uv.y = rect_pos_.pos.y + centr_pos_.uv.y;
        quad[0].uv.x = rect_pos_.pos.x + centr_pos_.uv.x;
        quad[0].uv.y = rect_pos_.pos.y + centr_pos_.uv.y + rect_pos_.size.y;
        quad[1].uv.x = rect_pos_.pos.x + centr_pos_.uv.x + rect_pos_.size.x;
        quad[1].uv.y = rect_pos_.pos.y + centr_pos_.uv.y + rect_pos_.size.y;
        quad[2].uv.x = rect_pos_.pos.x + centr_pos_.uv.x + rect_pos_.size.x;
        quad[2].uv.y = rect_pos_.pos.y + centr_pos_.uv.y;

        for (auto& qu : quad)
        {
            /// scale to GL_window
            qu.pos /= 2.f;
            /// scale to monitor_window
            qu.pos *= { static_cast<float>(screen_height) / screen_width, 1.f };
            qu.pos /= 5;
        }

        tri2              t0(quad[3], quad[1], quad[2]);
        tri2              t1(quad[0], quad[3], quad[1]);
        std::vector<tri2> quad_tri;
        quad_tri.push_back(t0);
        quad_tri.push_back(t1);
        return quad_tri;
    }

    vertex centr_pos_; /// centr of cell
    // float    size_;          /// size of cell
    rect     rect_pos_;      /// position of texture
    texture* tex_ = nullptr; /// texture for cell
};

struct figure
{
    // figure() {}
    figure(block c, std::vector<vertex>& arr, vec2 pos)
        : cell(c)
        , pos{ arr }
        , fig_center(pos)
    {
    }

    std::vector<tri2> build_all_tr()
    {
        std::vector<tri2>  vec_tr;
        std::vector<block> fig_block;

        for (int i = 0; i < 4; i++)
        {
            block             b(pos[i], cell.rect_pos_, cell.tex_);
            std::vector<tri2> res = b.build_block();
            std::copy_n(begin(res), 2, std::back_inserter(vec_tr));
        }

        return vec_tr;
    }
    block               cell;       /// one block of figure
    vec2                fig_center; /// center of all figure
    std::vector<vertex> pos;        /// position of cell for figure
};

int main()
{

    std::unique_ptr<engine, void (*)(engine*)> engine(create_engine(),
                                                      destroy_engine);

    const std::string error = engine->initialize("");
    if (!error.empty())
    {
        std::cerr << error << std::endl;
        return EXIT_FAILURE;
    }

    texture* texture = engine->create_texture("blocks1.png");

    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }
    const std::uint32_t tex_width  = texture->get_width();
    const std::uint32_t tex_height = texture->get_height();

    // block
    vertex            f_pos{ -0.0, -0.0, 0.0, 0.0 };
    rect              r{ { 0, 0 }, { 1.f / 7.f, 1 } };
    block             f(f_pos, r, texture);
    std::vector<tri2> t_res = f.build_block();
    vertex_buffer*    one_quad_buff =
        engine->create_vertex_buffer(&t_res[0], t_res.size());

    // end_block

    // figure
    vec2                central_pos(0.5f, 0.5f);
    std::vector<vertex> res1;

    std::copy_n(begin(figures_coord) + n, 4, std::back_inserter(res1));

    figure fig_I(f, res1, central_pos);

    std::vector<tri2> res{ fig_I.build_all_tr() };

    vertex_buffer* vert_buff =
        engine->create_vertex_buffer(&res[0], res.size());

    // end_figure

    vec2 current_pos(
        0.0f, 0.0f); // current_pos(0.8f, 1.3f); // current_pos(0.0f, 0.0f);
    const float pi = 3.1415926f;
    float       current_direction{ 0.f };
    float       current_scale{ 1.f };

    bool continue_loop = true;
    while (continue_loop)
    {
        event game_event;
        bool  check = true;
        while (engine->read_event(game_event) && check)
        {
            switch (game_event.key)
            {
                case keys::exit:
                    continue_loop = false;
                    break;
                case keys::right:
                    if (engine->is_key_down(keys::right))
                    {
                        std::cout << "keys::right" << std::endl;
                        current_pos.x += 0.05f;
                        // check_border(current_pos.x);
                        check = false;
                        break;

                        // game_event.is_down == false;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        current_pos.x -= 0.05f;
                        // check_border(current_pos.x);
                        check = false;
                        break;
                    }
                case keys::rotate:
                    if (engine->is_key_down(keys::rotate))
                    {
                        std::cout << "keys::rotate" << std::endl;
                        current_direction += pi / 2.f;
                        if (!(std::fmod(current_direction, pi) < 0.01f))
                            current_scale = static_cast<float>(screen_height) /
                                            static_cast<float>(screen_width);
                        else
                            current_scale = 1.f;
                        check = false;
                        break;
                        // current_direction = pi / 2.f;
                    }
                default:
                    break;
            }
        }
        vec2   centr_pos(0.0f, -0.0f);
        matrix move_cen = matrix::move(centr_pos);
        vec2   back_pos(0.0f, 0.0f);
        matrix move_back = matrix::move(back_pos);
        matrix move      = matrix::move(current_pos);
        matrix aspect    = matrix::scale(current_scale, 1);
        matrix rot       = matrix::rotation(current_direction);
        matrix m         = move_cen * rot * move_back * move * aspect;

        // engine->render_tetris(*one_quad_buff, f.tex_);

        // engine->render_tetris(*vert_buff, fig_I.cell.tex_);
        engine->render_tet(*vert_buff, texture, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

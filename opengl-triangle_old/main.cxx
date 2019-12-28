#include "engine.hxx"
#include "texture_gl_es20.hxx"

#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

const int m             = 10;
const int n             = 10;
int       fild[m][n]    = { 0 };
int       figures[7][4] = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // S
    4, 3, 6, 5, // Z 3, 5, 4, 6,
    4, 3, 5, 7, // T 3, 5, 4, 7,
    2, 3, 5, 7, // L
    6, 5, 3, 7, // J
    2, 3, 4, 5, // O
};
/// global
const int text_size = 18;
const int quad_size = 20;
vec2      a[4];
float     first_pos;

/// create array with coordinates all tetris figures
vec2 figures_coord[7][4];
void fill_tetris_fig(int fig_array[7][4])
{
    for (int i = 0; i < 7; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            figures_coord[i][j].x = fig_array[i][j] % 2;
            figures_coord[i][j].y = fig_array[i][j] / 2;
        }
    }
}

void draw_one_fig(std::vector<tri2>& vec_tr,
                  size_t             n) // set all triangles for 1 figure
{
    assert(n >= 0); // type of figure
    assert(n < 7);
    vec2 b[4];
    for (int i = 0; i < 4; i++)
    {
        b[i].x = figures_coord[n][i].x * quad_size;
        b[i].y = figures_coord[n][i].y * quad_size;
    }

    float x0 = (width - quad_size) / 2;
    float y0 = heigh - 4 * quad_size;
    ///   1            2
    ///   *------------*
    ///   |           /|
    ///   |         /  |
    ///   |      P/    |  // P - pos_ or center of sprite
    ///   |     /      |
    ///   |   /        |
    ///   | /          |
    ///   *------------*
    ///   0            3
    ///
    for (int i = 0; i < 4; i++)
    {
        // you can change colore of texture with shift uv coord
        v2   v00{ b[i].x + x0, b[i].y + y0, 0, 0 };
        v2   v01{ b[i].x + x0, b[i].y + y0 + quad_size, 0, text_size };
        v2   v02{ b[i].x + x0 + quad_size, b[i].y + y0 + quad_size, text_size,
                text_size };
        v2   v03{ b[i].x + x0 + quad_size, b[i].y + y0, text_size, 0 };
        tri2 t0(v00, v01, v02);
        tri2 t1(v03, v00, v02);
        vec_tr.push_back(t0);
        vec_tr.push_back(t1);
    }
}

tri2 transform_pixel_coord_to_GL(size_t tex_w, size_t tex_h, tri2& t)
{
    tri2 n;
    n.v[0].pos.x = t.v[0].pos.x * 2 / width - 1.0f;
    n.v[0].pos.y = t.v[0].pos.y * 2 / heigh - 1.0f;
    n.v[1].pos.x = t.v[1].pos.x * 2 / width - 1.0f;
    n.v[1].pos.y = t.v[1].pos.y * 2 / heigh - 1.0f;
    n.v[2].pos.x = t.v[2].pos.x * 2 / width - 1.0f;
    n.v[2].pos.y = t.v[2].pos.y * 2 / heigh - 1.0f;

    n.v[0].uv.x = t.v[0].uv.x * 2 / width - 1.0f;
    n.v[0].uv.y = t.v[0].uv.y * 2 / heigh - 1.0f;
    n.v[1].uv.x = t.v[1].uv.x * 2 / width - 1.0f;
    n.v[1].uv.y = t.v[1].uv.y * 2 / heigh - 1.0f;
    n.v[2].uv.x = t.v[2].uv.x * 2 / width - 1.0f;
    n.v[2].uv.y = t.v[2].uv.y * 2 / heigh - 1.0f;
    return n;
}
void check_border(float& x)
{
    float dx = quad_size / static_cast<float>(width);
    if (x < -1 + dx)
        x = -1 + dx;
    else if (x > 1.f - 3 * dx)
        x = 1.f - 3 * dx;
    else
        return;
}

int main()
{
    fill_tetris_fig(figures);

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

    std::vector<tri2> t;
    size_t            figure_type = 2;
    draw_one_fig(t, figure_type);

    std::vector<tri2> t_end;
    for (auto var : t)
    {
        // 7 - number of blocks in png
        tri2 t1 = transform_pixel_coord_to_GL(tex_width / 7, tex_height, var);
        t_end.push_back(t1);
    }
    vertex_buffer* vert_buff =
        engine->create_vertex_buffer(&t_end[0], t_end.size());

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
                        check_border(current_pos.x);
                        check = false;
                        break;

                        // game_event.is_down == false;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        current_pos.x -= 0.05f;
                        check_border(current_pos.x);
                        check = false;
                        break;
                    }
                case keys::rotate:
                    if (engine->is_key_down(keys::rotate))
                    {
                        std::cout << "keys::rotate" << std::endl;
                        current_direction += pi / 2.f;
                        //                        if
                        //                        (!(std::fmod(current_direction,
                        //                        pi) < 0.01f))
                        //                            current_scale =
                        //                            static_cast<float>(heigh)
                        //                            /
                        //                                            static_cast<float>(width);
                        //                        else
                        //                            current_scale = 1.f;
                        check = false;
                        break;
                        //                current_direction = pi / 2.f;
                    }
                default:
                    break;
            }
        }
        vec2   centr_pos(0.0f, -0.9f);
        matrix move_cen = matrix::move(centr_pos);
        vec2   back_pos(0.0f, 0.9f);
        matrix move_back = matrix::move(back_pos);
        matrix move      = matrix::move(current_pos);
        matrix aspect    = matrix::scale(1, current_scale);
        matrix rot       = matrix::rotation(current_direction);
        matrix m         = move_cen * rot * move_back * move * aspect;

        // engine->render_tetris(*vert_buff, texture);
        engine->render_tet(*vert_buff, texture, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

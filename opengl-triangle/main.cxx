#include "engine.hxx"
#include "texture_gl_es20.hxx"

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
int                 n             = 20;
std::vector<vertex> figures_coord = {
    { -0.5, 2.0, 0.0, 0.0 },
    { -0.5, 1.0, 0.0, 0.0 },
    { -0.5, 0.0, 0.0, 0.0 },
    { -0.5, -1.0, 0.0, 0.0 }, // I
    //
    { 0.0, 1.5, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 },
    { -1.0, 0.5, 0.0, 0.0 },
    { -1.0, -0.5, 0.0, 0.0 }, // Z

    //    { -1.0, -0.5, 0.0, 0.0 }, { -1.0, 0.5, 0.0, 0.0 },
    //    { 0.0, -0.5, 0.0, 0.0 },  { 0.0, 0.5, 0.0, 0.0 }, // S

    { 0.0, -0.5, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 },
    { -1.0, 0.5, 0.0, 0.0 },
    { -1.0, 1.5, 0.0, 0.0 }, // S

    { -1.0, -0.5, 0.0, 0.0 },
    { -1.0, 0.5, 0.0, 0.0 },
    { -1.0, 1.5, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 }, // T

    { -1.0, 1.5, 0.0, 0.0 },
    { -1.0, 0.5, 0.0, 0.0 },
    { -1.0, -0.5, 0.0, 0.0 },
    { 0.0, -0.5, 0.0, 0.0 }, // L

    { 0.0, 1.5, 0.0, 0.0 },
    { 0.0, 0.5, 0.0, 0.0 },
    { 0.0, -0.5, 0.0, 0.0 },
    { -1.0, -0.5, 0.0, 0.0 }, // J

    { -1.0, 1.0, 0.0, 0.0 },
    { -1.0, 0.0, 0.0, 0.0 },
    { 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0 } // O
};

/// global
const float text_size = 18.f / 640.f;
const float quad_size = 20.f;
vec2        a[4];
float       first_pos;

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
struct rect
{
    vec2 pos;
    vec2 size;
};
struct figure
{
    figure() {}
    figure(vertex v00, vertex v01, vertex v02, vertex v03, vec2 pos)
        : pos{ vertex(v00), vertex(v01), vertex(v02), vertex(v03) }
        , centr_(pos)

    {
    }

    std::vector<tri2> build_all_tr()
    {
        std::vector<tri2> vec_tr;

        float size_uv_x = 1 / 7.f; /// static_cast<float>(width);
        float size_uv_y = 1;       /// static_cast<float>(heigh);
        float step      = 1.f;

        /// remember in OpenGL texture lower left angle is (0, 0) coordinate
        for (int i = 0; i < 4; i++)
        {
            // you can change colore of texture with shift uv coord
            ///   0            1
            ///   *------------*
            ///   |           /|
            ///   |         /  |
            ///   |      P/    |  // P - pos_ or center
            ///   |     /      |
            ///   |   /        |
            ///   | /          |
            ///   *------------*
            ///   3            2
            ///
            ///
            vertex vertexes[4];
            vertexes[0].pos = { centr_.x + pos[i].pos.x,
                                centr_.y + pos[i].pos.y };
            vertexes[1].pos = { centr_.x + pos[i].pos.x + step,
                                centr_.y + pos[i].pos.y };
            vertexes[2].pos = { centr_.x + pos[i].pos.x + step,
                                centr_.y + pos[i].pos.y - step };
            vertexes[3].pos = { centr_.x + pos[i].pos.x,
                                centr_.y + pos[i].pos.y - step };

            vertexes[0].uv = { pos[i].uv.x, pos[i].uv.y + size_uv_y };
            vertexes[1].uv = { pos[i].uv.x + size_uv_x,
                               pos[i].uv.y + size_uv_y };
            vertexes[2].uv = { pos[i].uv.x + size_uv_x, pos[i].uv.y };
            vertexes[3].uv = { pos[i].uv.x, pos[i].uv.y };

            // to GL coord, scale 2;
            for (auto& v : vertexes)
            {
                v.pos /= 2.f;
            }
            tri2 t0(vertexes[3], vertexes[1], vertexes[2]);
            tri2 t1(vertexes[0], vertexes[3], vertexes[1]);
            vec_tr.push_back(t0);
            vec_tr.push_back(t1);
        }
        return vec_tr;
    }

    vec2   centr_;
    vertex pos[4];
    rect   text_uv_;
};

int main()
{

    vec2   central_pos(0.5f, 0.5f);
    figure fig_I(figures_coord[n], figures_coord[n + 1], figures_coord[n + 2],
                 figures_coord[n + 3], central_pos);

    std::vector<tri2> res;
    res = fig_I.build_all_tr();

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

    vertex_buffer* vert_buff =
        engine->create_vertex_buffer(&res[0], res.size());

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
                    //                case keys::right:
                    //                    if (engine->is_key_down(keys::right))
                    //                    {
                    //                        std::cout << "keys::right" <<
                    //                        std::endl; current_pos.x += 0.05f;
                    //                        check_border(current_pos.x);
                    //                        check = false;
                    //                        break;

                    //                        // game_event.is_down == false;
                    //                    }
                    //                case keys::left:
                    //                    if (engine->is_key_down(keys::left))
                    //                    {
                    //                        std::cout << "keys::left" <<
                    //                        std::endl; current_pos.x -= 0.05f;
                    //                        check_border(current_pos.x);
                    //                        check = false;
                    //                        break;
                    //                    }
                    //                case keys::rotate:
                    //                    if (engine->is_key_down(keys::rotate))
                    //                    {
                    //                        std::cout << "keys::rotate" <<
                    //                        std::endl; current_direction += pi
                    //                        / 2.f;
                    //                        //                        if
                    //                        // (!(std::fmod(current_direction,
                    //                        //                        pi) <
                    //                        0.01f))
                    //                        // current_scale =
                    //                        // static_cast<float>(heigh)
                    //                        //                            /
                    //                        // static_cast<float>(width);
                    //                        //                        else
                    //                        // current_scale = 1.f; check =
                    //                        false; break;
                    //                        // current_direction = pi / 2.f;
                    //                    }
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

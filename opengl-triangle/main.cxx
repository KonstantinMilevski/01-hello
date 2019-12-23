#include "engine.hxx"
#include "texture_gl_es20.hxx"

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
    3, 5, 4, 6, // Z 3, 5, 4, 6,
    3, 5, 4, 7, // T 3, 5, 4, 7,
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5, // O
};
const int text_size = 18;
const int quad_size = 36;
vec2      a[4];
/// create coordinates all tetris figures
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

    assert(n >= 0); // задаём тип тетрамино
    assert(n < 7);
    vec2 b[4];
    for (int i = 0; i < 4; i++)
    {

        b[i].x = figures_coord[n][i].x * quad_size;
        b[i].y = figures_coord[n][i].y * quad_size;
    }
    for (int i = 0; i < 4; i++)

    {
        v2 v01{ b[i].x, b[i].y, 0, 0 };
        v2 v02{ b[i].x, b[i].y + quad_size, 0, text_size };
        v2 v03{ b[i].x + quad_size, b[i].y + quad_size, text_size, text_size };
        v2 v04{ b[i].x + quad_size, b[i].y, text_size, 0 };
        tri2 t0(v01, v02, v03);
        tri2 t1(v04, v01, v03);
        vec_tr.push_back(t0);
        vec_tr.push_back(t1);
    }
    //    for (int i = 0; i < 4; i++)
    //    {
    //         Устанавливаем позицию каждого кусочка тетрамино
    //         sprite.setPosition(a[i].x * 18, a[i].y * 18);
    //         Отрисовка спрайта
    //         window.draw(sprite);
    //    }
}
// tri2 scale_size(size_t w, size_t h, tri2 t)
//{
//    tri2  n;
//    float a = static_cast<float>(w / h);

//    n.v[0].pos.x = t.v[0].pos.x;
//    n.v[0].pos.y = t.v[0].pos.y / 3.5;
//    n.v[1].pos.x = t.v[1].pos.x;
//    n.v[1].pos.y = t.v[1].pos.y / 3.5;
//    n.v[2].pos.x = t.v[2].pos.x;
//    n.v[2].pos.y = t.v[2].pos.y / 3.5;

//    n.v[0].uv.x = t.v[0].uv.x;
//    n.v[0].uv.y = t.v[0].uv.y * a;
//    n.v[1].uv.x = t.v[1].uv.x;
//    n.v[1].uv.y = t.v[1].uv.y * a;
//    n.v[2].uv.x = t.v[2].uv.x;
//    n.v[2].uv.y = t.v[2].uv.y * a;

//    return n;
//}
tri2 transform_coord_to_GL(size_t tex_w, size_t tex_h, tri2& t)
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

using namespace std;

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
    size_t            figure_type = 3;
    draw_one_fig(t, figure_type);

    std::vector<tri2> t_end;
    for (auto var : t)
    {

        tri2 t1 =
            transform_coord_to_GL(tex_width / tex_height, tex_height, var);
        t_end.push_back(t1);
    }

    vec2        current_pos(0.f, 0.f);
    float       current_direction(0.f);
    const float pi = 3.1415926f;

    bool continue_loop = true;
    while (continue_loop)
    {
        keys game_key;

        while (engine->read_event(game_key))
        {
            switch (game_key)
            {
                case keys::exit:
                    continue_loop = false;
                    break;
                default:
                    break;
            }

            if (engine->is_key_down(om::keys::right))
            {
                current_tank_pos.x += 0.01f;
                current_tank_direction = -pi / 2.f;
            }
        }

        vertex_buffer* vert_buff =
            engine->create_vertex_buffer(&t_end[0], t_end.size());

        engine->render_tetris(*vert_buff, texture);

        engine->swap_buffer();
    };

    engine->uninitialize();
    return 0;
}

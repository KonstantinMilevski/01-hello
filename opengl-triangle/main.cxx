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
vec2      a[4], b[4];

void draw_one_fig(std::vector<tri2>& vec_tr) // set all triangles for 1 figure
{

    int n = 3; // задаём тип тетрамино
    for (int i = 0; i < 4; i++)
    {
        a[i].x = figures[n][i] % 2 + 5;
        a[i].y = figures[n][i] / 2 + 5;
    }
    for (int i = 0; i < 4; i++)
    {
        b[i].x = a[i].x * quad_size;
        b[i].y = a[i].y * quad_size;
    }
    for (int i = 0; i < 4; i++)

    {

        v2 v01{ b[i].x, b[i].y, 0, 0 };
        v2 v02{ b[i].x, b[i].y + quad_size, 0, text_size };
        v2 v03{ b[i].x + quad_size, b[i].y + quad_size, text_size, text_size };
        v2 v04{ b[i].x + quad_size, b[i].y, text_size, 0 };
        tri2 t0(v01, v02, v03);
        tri2 t1(v04, v02, v03);
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
        }

        std::ifstream file("vert_and_tex_coord.txt");
        assert(!!file);

        //        triangle tr1, tr2;

        //        file >> tr1;
        //        file >> tr2;
        //        triangle q1 = scale_size(126, 18, tr1);
        //        triangle q2 = scale_size(126, 18, tr2);
        //        triangle q1({ 200, 200, 0, 0 }, { 200, 218, 0, 18 },
        //                    { 326, 200, 126, 18 });
        //        triangle q2({ 326, 218, 126, 18 }, { 200, 218, 0, 18 },
        //                    { 326, 200, 126, 18 });

        //        triangle q1({ 200, 200, 0, 0 }, { 200, 236, 0, 18 },
        //                    { 236, 200, 18, 18 });
        //        triangle q2({ 236, 236, 18, 18 }, { 200, 236, 0, 18 },
        //                    { 236, 200, 18, 18 });
        //        triangle q5({ 300, 300, 0, 0 }, { 300, 318, 0, 18 },
        //                    { 318, 300, 18, 18 });
        //        triangle q6({ 318, 318, 0, 0 }, { 300, 318, 0, 18 },
        //                    { 318, 300, 18, 18 });
        //        triangle q3({ 0, 0, 0, 0 }, { 000, 18, 0, 18 }, { 18, 000, 18,
        //        18 }); triangle q4({ 18, 18, 0, 0 }, { 000, 18, 0, 18 }, { 18,
        //        000, 18, 18 });

        // std::vector<triangle> t{ q1, q2, q3, q4, q5, q6 };
        ////
        //        std::vector<triangle> t2;

        //        tri2 q2 = scale_size(126, 18, tr2);
        //        t_end   = { tr1, tr2 };

        std::vector<tri2> t;
        draw_one_fig(t);

        std::vector<tri2> t_end;
        for (auto var : t)
        {

            tri2 t1 =
                transform_coord_to_GL(tex_width / tex_height, tex_height, var);
            t_end.push_back(t1);
        }

        vertex_buffer* vert_buff =
            engine->create_vertex_buffer(&t_end[0], t_end.size());

        engine->render_tetris(*vert_buff, texture);

        engine->swap_buffer();
    };

    engine->uninitialize();
    return 0;
}

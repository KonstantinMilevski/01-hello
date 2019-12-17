#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#include "engine.hxx"

void      transform_coor();
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
const int quad_size = 18;
vec2      a[4], b[4];

void draw_one_fig(std::vector<triangle>& vec_tr)
{
    // std::vector<triangle> vec_tr;
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
        vertex   v0{ b[i].x, b[i].y, 0, 0 };
        vertex   v1{ b[i].x, b[i].y + quad_size, 0, text_size };
        vertex   v2{ b[i].x + quad_size, b[i].y + quad_size, text_size,
                   text_size };
        vertex   v3{ b[i].x + quad_size, b[i].y, text_size, 0 };
        triangle t0(v0, v1, v2);
        triangle t1(v3, v0, v2);
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
triangle scale_size(size_t w, size_t h, triangle t)
{
    triangle n;
    float    a = static_cast<float>(w / h);
    n.v[0].x   = t.v[0].x;
    n.v[0].y   = t.v[0].y / 3.5;
    n.v[1].x   = t.v[1].x;
    n.v[1].y   = t.v[1].y / 3.5;
    n.v[2].x   = t.v[2].x;
    n.v[2].y   = t.v[2].y / 3.5;

    n.v[0].tx = t.v[0].tx;
    n.v[0].ty = t.v[0].ty * a;
    n.v[1].tx = t.v[1].tx;
    n.v[1].ty = t.v[1].ty * a;
    n.v[2].tx = t.v[2].tx;
    n.v[2].ty = t.v[2].ty * a;
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

    texture* texture = engine->create_texture("tank.png");
    if (nullptr == texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

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

        triangle tr1, tr2;

        file >> tr1;
        file >> tr2;
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
        std::vector<triangle> t2;
        draw_one_fig(t2);
        engine->render_two_triangles(t2);
        // triangle q2 = scale_size(126, 18, tr2);

        //        engine->render_text_triangle(q1);
        //        engine->render_text_triangle(q2);
        engine->swap_buffer();
    };

    engine->uninitialize();
    return 0;
}

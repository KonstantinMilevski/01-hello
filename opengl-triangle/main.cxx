#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>

#include "engine.hxx"
void transform_coor();
int  figures[7][4] = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // S
    3, 5, 4, 6, // Z
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5, // O
};
struct Point
{
    int x, y;
} a[4], b[4];
void draw_one_fig()
{
    int n = 3; // задаём тип тетрамино
    for (int i = 0; i < 4; i++)
    {
        a[i].x = figures[n][i] % 2;
        a[i].y = figures[n][i] / 2;
    }
    for (int i = 0; i < 4; i++)
    {
        // Устанавливаем позицию каждого кусочка тетрамино
        // sprite.setPosition(a[i].x * 18, a[i].y * 18);
        // Отрисовка спрайта
        // window.draw(sprite);
    }
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
    bool continue_loop = true;
    while (continue_loop)
    {
        int key = 0;

        while (engine->read_input(key))
        {
            switch (key)
            {
                case 0:
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
        triangle q1({ 200, 200, 0, 0 }, { 200, 236, 0, 18 },
                    { 236, 200, 18, 18 });
        triangle q2({ 236, 236, 18, 18 }, { 200, 236, 0, 18 },
                    { 236, 200, 18, 18 });
        // triangle q2 = scale_size(126, 18, tr2);

        engine->render_text_triangle(q1);
        engine->render_text_triangle(q2);
        engine->swap_buffer();
    };

    engine->uninitialize();
    return 0;
}

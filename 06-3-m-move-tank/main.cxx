#include "engine.hxx"

#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <memory>

v0 blend_vertex(const v0& left, const v0& right, const float& t)
{
    v0 v;
    v.p.x = left.p.x + t * (right.p.x - left.p.x);
    v.p.y = left.p.y + t * (right.p.y - left.p.y);
    return v;
}

tri0 blend(const tri0& left, const tri0& right, const float& t)
{

    tri0 tr;
    tr.v[0] = blend_vertex(left.v[0], right.v[0], t);
    tr.v[1] = blend_vertex(left.v[1], right.v[1], t);
    tr.v[2] = blend_vertex(left.v[2], right.v[2], t);
    return tr;
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

    vertex_buffer* vertex_buf = nullptr;

    std::ifstream file("vert_tex_color.txt");
    if (!file)
    {
        std::cerr << "can't load vert_tex_color.txt\n";
        return EXIT_FAILURE;
    }
    else
    {
        std::array<tri2, 2> tr;
        file >> tr[0] >> tr[1];
        vertex_buf = engine->create_vertex_buffer(&tr[0], tr.size());
        if (vertex_buf == nullptr)
        {
            std::cerr << "can't create vertex buffer\n";
            return EXIT_FAILURE;
        }
    }

    bool continue_loop = true;

    vec2        current_tank_pos(0.f, 0.f);
    const float pi = 3.1415926f;
    float       current_tank_direction(0);

    while (continue_loop)
    {
        event event;

        while (engine->read_event(event))
        {
            std::cout << event << std::endl;
            switch (event)
            {
                case event::turn_off:
                    continue_loop = false;
                    break;
                default:

                    break;
            }
        }
        if (engine->is_key_down(keys::left))
        {
            current_tank_pos.x -= 0.02f;
            current_tank_direction = pi * 0.5f;
        }
        else if (engine->is_key_down(keys::right))
        {
            current_tank_pos.x += 0.02f;
            current_tank_direction = -pi / 2.f;
        }
        else if (engine->is_key_down(keys::up))
        {
            current_tank_pos.y += 0.02f;
            current_tank_direction = -pi;
        }
        else if (engine->is_key_down(keys::down))
        {
            current_tank_pos.y -= 0.02f;
            current_tank_direction = 0.f;
        }
        float s_alfa = std::sin(0.25 * pi);
        if (engine->is_key_down(keys::down) && engine->is_key_down(keys::right))
        {
            current_tank_pos.x += 0.02f * s_alfa;
            current_tank_pos.y -= 0.02f * s_alfa * 640.f / 480.f;
            current_tank_direction = -pi * 0.25f;
        }
        if (engine->is_key_down(keys::down) && engine->is_key_down(keys::left))
        {
            current_tank_pos.x -= 0.02f * s_alfa;
            current_tank_pos.y -= 0.02f * s_alfa * 640.f / 480.f;
            current_tank_direction = pi * 0.25f;
        }
        if (engine->is_key_down(keys::up) && engine->is_key_down(keys::left))
        {
            current_tank_pos.x -= 0.02f * s_alfa;
            current_tank_pos.y += 0.02f * s_alfa * 2.f;
            current_tank_direction = pi * 0.75f;
        }
        if (engine->is_key_down(keys::up) && engine->is_key_down(keys::right))
        {
            current_tank_pos.x += 0.02f * s_alfa;
            current_tank_pos.y += 0.02f * s_alfa * 2.f;
            current_tank_direction = -pi * 0.75f;
        }

        mat2x3 move   = mat2x3::move(current_tank_pos);
        mat2x3 aspect = mat2x3::scale(1, 640.f / 480.f);
        mat2x3 rot    = mat2x3::rotation(current_tank_direction);
        mat2x3 m      = rot * move * aspect;

        engine->render(*vertex_buf, texture, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return EXIT_SUCCESS;
}

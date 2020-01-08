#include "engine.hxx"
#include "field.hxx"
#include "texture_gl_es20.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>

int figures[7][4] = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // S
    3, 5, 4, 6, // Z
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5, // O
};
std::array<size_t, 4> fig_S{ 2, 4, 5, 7 };
/// each position is centr
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

    texture* text_main_bar = engine->create_texture("blocks1.png");

    if (nullptr == text_main_bar)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }
    texture* back_up_texture = engine->create_texture("frame.png");
    if (nullptr == back_up_texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }
    /// block start
    rect bloc_pos({ 0.f, 0.f }, { cell_size, cell_size });
    rect bloc_text({ 0.f, 0.f }, { 1.f / 7, 1.f });

    block bl_01({ { 5.f, 5.f }, { cell_size, cell_size } }, bloc_text, nullptr);
    block bl_02(bloc_pos, bloc_text, text_main_bar);
    block bl_03(bloc_pos, bloc_text, text_main_bar);

    std::vector<vertex> one_block_vert = bl_01.build_block();
    vertex_buffer*      one_block_vert_buf =
        engine->create_vertex_buffer(&one_block_vert[0], one_block_vert.size());
    /// block end

    /// field srart
    field main_field(field_width, field_height);
    main_field.set_block(bl_03, field_height * field_width - 1);
    main_field.set_block(bl_02, 0);
    figure f_01(fig_S);
    main_field.set_figure(f_01, bl_01, 25);

    std::vector<vertex> arr_block_vert = main_field.occupied_cells();
    vertex_buffer*      arr_block_vert_buf =
        engine->create_vertex_buffer(&arr_block_vert[0], arr_block_vert.size());

    /// field ends

    // vec2        current_pos(main_field.get_position(pos));
    const float pi = 3.1415926f;
    float       current_direction{ 0.f };
    int         pos           = 0;
    bool        continue_loop = true;
    while (continue_loop)
    {
        //        float time = engine->get_time_from_init();
        //        timer += time;
        //        if (timer >= dt)
        //        {
        //            current_pos.y -= cell_size * 0.01;
        //            timer = 0.f;
        //            if (!check_border(current_pos))
        //            {
        //                current_pos.y += cell_size * 0.01;
        //            }
        //        }
        event game_event;
        while (engine->read_event(game_event))
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
                        pos++;
                        main_field.clear_cells();
                        if (main_field.set_figure(f_01, bl_01, pos))
                            break;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        pos--;
                        main_field.clear_cells();
                        if (main_field.set_figure(f_01, bl_01, pos))
                            break;
                        break;
                    }
                case keys::rotate:
                    if (engine->is_key_down(keys::rotate))
                    {
                        std::cout << "keys::rotate" << std::endl;
                        current_direction += pi * 0.5f;
                        break;
                    }
                default:
                    break;
            }
        }
        arr_block_vert     = main_field.occupied_cells();
        arr_block_vert_buf = engine->create_vertex_buffer(
            &arr_block_vert[0], arr_block_vert.size());

        //        //  vec2   start_pos(main_field.get_position(4));
        //        //(0.0f, 1.0f - 0.5);
        vec2 zero_pos(cell_size * field_width * (-0.5),
                      cell_size * field_height * (-0.5));
        // vec2   zero_pos(-1.f, -1.f);
        matrix zero = matrix::move(zero_pos);
        // matrix move          = matrix::move(current_pos);
        matrix screen_aspect =
            matrix::scale(window_scale * 0.009f, 1.0f * 0.009f);
        // matrix rot           = matrix::rotation(current_direction);
        matrix m = zero * screen_aspect;
        //        engine->render_tet(*one_block_vert_buf, text_main_bar,
        //                           matrix::scale(window_scale, 1.0f) *
        //                               matrix::scale(0.01f, .01f));
        engine->render_tet(*arr_block_vert_buf, text_main_bar, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

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
std::array<size_t, 4> fig_S{ 10, 20, 21, 31 };
std::array<size_t, 4> fig_T{ 1, 10, 11, 12 };
std::array<size_t, 4> fig_Z{ 3, 5, 4, 6 };
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
    //    block bl_02(bloc_pos, bloc_text, text_main_bar);
    //    block bl_03(bloc_pos, bloc_text, text_main_bar);

    //    std::vector<vertex> one_block_vert = bl_01.build_block();

    /// field srart
    field main_field(field_width, field_height);

    //    figure f_01(fig_Z);
    figure f_02(fig_S);

    std::vector<vertex> arr_block_vert = main_field.occupied_cells();
    vertex_buffer*      arr_block_vert_buf;

    int    d_pos       = 0;
    float  start_timer = engine->get_time_from_init();
    float  dt          = 0.2;
    size_t count       = 0;
    f_02.figure_change_position(164);
    bool   rotate        = false;
    bool   continue_loop = true;
    bool   start_game    = true;
    figure playing_figure;

    while (continue_loop)
    {
        if (start_game)
        {
            playing_figure = f_02;

            start_game = false;
        }
        float current_time = engine->get_time_from_init();
        float timer        = current_time - start_timer;
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
                        d_pos += 1;
                        break;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        d_pos -= 1;
                        break;
                    }
                case keys::rotate:
                    if (engine->is_key_down(keys::rotate))
                    {
                        std::cout << "keys::rotate" << std::endl;
                        rotate = true;
                        break;
                    }
                default:
                    break;
            }
        }
        figure prev = playing_figure;
        main_field.clear_position(playing_figure);

        playing_figure.figure_change_position(d_pos);
        if (!(main_field.check_field_border(playing_figure) &&
              main_field.check_empty_cell(playing_figure) &&
              main_field.check_figure_horiszont(prev, playing_figure)))
        {
            playing_figure = prev;
        }
        if (rotate)
            playing_figure.figure_rotate();

        main_field.set_figure(playing_figure, bl_01);
        if (timer >= dt)
        {
            // main_field.clear_position(playing_figure);
            d_pos -= 10;
            //            count++;
            //            if (count > 1)
            //            {
            //                count = 0;
            //                figure f_03(fig_T);
            //                playing_figure = f_03;
            //                playing_figure.figure_change_position(164);
            //            }
            //            main_field.clear_position(playing_figure);
            //            playing_figure.figure_change_position(d_pos);
            //            main_field.set_figure(prev, bl_01);
            //            start_timer = current_time;
        }

        arr_block_vert     = main_field.occupied_cells();
        arr_block_vert_buf = engine->create_vertex_buffer(
            &arr_block_vert[0], arr_block_vert.size());
        rotate = false;
        d_pos  = 0;

        //        vertex_buffer*      back_block_vert_buf =
        //        engine->create_vertex_buffer(
        //            &back_block_vert[0], back_block_vert.size());

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
        // engine->render_tet(*back_block_vert_buf, text_main_bar, m);
        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

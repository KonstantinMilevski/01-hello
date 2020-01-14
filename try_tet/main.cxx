#include "engine.hxx"
#include "field.hxx"
#include "texture_gl_es20.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>

std::array<std::array<size_t, 4>, 7> figures = {
    1, 3, 5, 7, // I
    2, 4, 5, 7, // S
    3, 5, 4, 6, // Z
    3, 5, 4, 7, // T
    2, 3, 5, 7, // L
    3, 5, 7, 6, // J
    2, 3, 4, 5, // O
};
/// left down coord texture
std::array<vec2, 7> second_texture_pos = { {
    { 0.f, 0.f },
    { 1.f / 7, 0.f },
    { 2.f / 7, 0.f },
    { 3.f / 7, 0.f },
    { 4.f / 7, 0.f },
    { 5.f / 7, 0.f },
    { 6.f / 7, 0.f },
} };

std::array<size_t, 4> select_figure(std::array<std::array<size_t, 4>, 7>& fig)
{

    std::array<size_t, 4> res;
    srand(time(0));
    const size_t rand_index = static_cast<size_t>(rand());
    const size_t row        = rand_index % 7;
    for (size_t i = 0; i < 4; i++)
    {
        res.at(i) = fig[row][i];
    }
    return res;
}
rect generate_texture_position(std::array<vec2, 7>& text_pos)
{

    const size_t rand_index    = static_cast<size_t>(rand());
    const size_t number        = rand_index % 7;
    vec2         next_text_pos = text_pos.at(number);
    /// texture size
    vec2 next_text_size = { 1.f / 7, 1.f };
    return { next_text_pos, next_text_size };
}

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
    rect  block_pos({ 0.f, 0.f }, { cell_size, cell_size });
    rect  block_text({ 1.f / 7, 0.f }, { 1.f / 7, 1.f });
    block main_block(block_pos, block_text);

    /// field srart
    size_t width_main_field  = 20;
    size_t height_main_field = 25;
    field  main_field(width_main_field, height_main_field);
    /// select first figure
    std::array<size_t, 4> start_coord = select_figure(figures);
    figure                start_figure(start_coord, width_main_field);

    std::vector<vertex> arr_block_vert;
    vertex_buffer*      arr_block_vert_buf;

    int   d_pos       = 0;
    float start_timer = engine->get_time_from_init();
    float dt          = 0.3;
    /// first figure start position '-4' row from top, center
    start_figure.figure_change_position(
        (height_main_field - 4) * width_main_field + width_main_field / 2);
    bool rotate        = false;
    bool continue_loop = true;
    bool start_game    = true;
    /// figure for main loop
    figure playing_figure;

    while (continue_loop)
    {
        if (start_game)
        {
            playing_figure = start_figure;

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

                case keys::down:
                    if (engine->is_key_down(keys::down))
                    {
                        std::cout << "keys::down" << std::endl;
                        dt = 0.01f;
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
              main_field.check_figure_horizont(prev, playing_figure)))
        {
            playing_figure = prev;
        }

        if (rotate)
        {
            prev = playing_figure;
            main_field.clear_position(playing_figure);
            playing_figure.figure_rotate(width_main_field);
            if (!(main_field.check_field_border(playing_figure) &&
                  main_field.check_empty_cell(playing_figure)))
            {
                playing_figure = prev;
            }
        }

        main_field.set_figure(playing_figure, main_block);

        if (timer >= dt)
        {
            prev  = playing_figure;
            d_pos = -width_main_field;
            main_field.clear_position(playing_figure);
            playing_figure.figure_change_position(d_pos);
            if (main_field.check_field_border(playing_figure) &&
                main_field.check_empty_cell(playing_figure))
            {
                main_field.set_figure(playing_figure, main_block);
            }
            else
            {
                main_field.set_figure(prev, main_block);
                std::array<size_t, 4> next_coord = select_figure(figures);
                figure                next_figure(next_coord, width_main_field);
                rect                  next_texture =
                    generate_texture_position(second_texture_pos);
                main_block.set_texture_pos(next_texture);
                playing_figure = next_figure;
                playing_figure.figure_change_position((height_main_field - 4) *
                                                          width_main_field +
                                                      width_main_field / 2);

                main_field.check_field();
                main_field.set_figure(playing_figure, main_block);
            }

            start_timer = current_time;
        }

        arr_block_vert     = main_field.occupied_cells();
        arr_block_vert_buf = engine->create_vertex_buffer(
            &arr_block_vert[0], arr_block_vert.size());
        rotate = false;
        dt     = 0.3;
        d_pos  = 0;

        vec2 zero_pos(cell_size * field_width * (-0.5),
                      cell_size * field_height * (-0.5));
        // vec2   zero_pos(-1.f, -1.f);
        matrix zero = matrix::move(zero_pos);
        // matrix move          = matrix::move(current_pos);
        matrix screen_aspect =
            matrix::scale(window_scale * 0.009f, 1.0f * 0.009f);
        // matrix rot           = matrix::rotation(current_direction);
        matrix m = zero * screen_aspect;

        engine->render_tet(*arr_block_vert_buf, text_main_bar, m);
        // engine->render_tet(*back_block_vert_buf, text_main_bar, m);
        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

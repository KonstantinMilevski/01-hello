#include "engine.hxx"
#include "texture_gl_es20.hxx"

#include <algorithm>
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

/// global
float        first_pos;
static float window_scale = static_cast<float>(screen_height) / screen_width;
/// field 10x25 GL from -0.5 to 0.5 gl_width     =1.f
/// field 10x25 GL from -0.1 to 0.1 gl_width     =2.f
size_t field_height = 20;
size_t field_width  = 10;
float  gl_height    = 2.f;
float  cell_size    = gl_height / field_height;
float  gl_width     = cell_size * field_width; // 2.f;
int    n            = 4;
float  left_border  = -gl_width * 0.5; // + cell_size * 0.5;
float  right_border = gl_width * 0.5;  // - cell_size * 0.5;
float  dt           = 1.f;
float  timer        = 0.f;

bool check_border(float& x)
{
    if (x < left_border + cell_size * 0.5 || x > right_border - cell_size * 0.5)
        return false;
    else
        return true;
}

struct rect
{
    rect()
        : pos{ 0.f, 0.f }
        , size{ 0.f, 0.f }
    {
    }
    rect(vec2 p, vec2 s)
        : pos{ p }
        , size{ s }
    {
    }
    vec2 pos;
    vec2 size;
};
struct block
{
    block() = default;
    block(vec2 pos, rect uv_pos, texture* tex)
        : centr_pos_(pos)
        , rect_pos_(uv_pos)
        , tex_(tex)
    {
    }
    std::vector<vertex> build_block()
    {
        std::array<vertex, 4> quad;
        // you can change colore of texture with shift uv coord
        ///   0            1
        ///   *------------*
        ///   |           /|
        ///   |         /  |
        ///   |      P/    |  // P - pos_ center - OpenGL (0,0)
        ///   |     /      |  // vertex 3 - texture (0,0)
        ///   |   /        |
        ///   | /          |
        ///   *------------*
        ///   3            2
        ///
        /// 0 left-up, clockwise
        /// vec2.pos
        quad[0].pos.x = centr_pos_.x - cell_size * 0.5;
        quad[0].pos.y = centr_pos_.y + cell_size * 0.5;
        quad[1].pos.x = centr_pos_.x + cell_size * 0.5;
        quad[1].pos.y = centr_pos_.y + cell_size * 0.5;

        quad[2].pos.x = centr_pos_.x + cell_size * 0.5;
        quad[2].pos.y = centr_pos_.y - cell_size * 0.5;
        quad[3].pos.x = centr_pos_.x - cell_size * 0.5;
        quad[3].pos.y = centr_pos_.y - cell_size * 0.5;
        /// vec2.uv, OpenGL texture lower left angle is (0, 0) coordinate
        quad[3].uv.x = rect_pos_.pos.x;
        quad[3].uv.y = rect_pos_.pos.y;
        quad[0].uv.x = quad[3].uv.x;
        quad[0].uv.y += rect_pos_.size.y;
        quad[1].uv.x += rect_pos_.size.x;
        quad[1].uv.y += rect_pos_.size.y;

        quad[2].uv.x += rect_pos_.size.x;
        quad[2].uv.y = quad[3].uv.y;

        for (auto& qu : quad)
        {
            /// scale to GL_window
            // qu.pos /= 2.f;
        }

        std::vector<vertex> quad_tri = { quad[3], quad[1], quad[2],
                                         quad[0], quad[3], quad[1] };
        return quad_tri;
    }

    vec2     centr_pos_;     /// centr of cell
    rect     rect_pos_;      /// position of texture
    texture* tex_ = nullptr; /// texture for cell
};

struct figure
{
    std::vector<block> all_figure;
    // figure() {}
    figure(block c, std::vector<vec2>& arr, vec2 pos)
        : cell_(c)
        , cell_pos{ arr }
        , fig_centr_pos(pos)
    {
        block b00(arr[0], c.rect_pos_, c.tex_);
        block b01(arr[1], c.rect_pos_, c.tex_);
        block b02(arr[2], c.rect_pos_, c.tex_);
        block b03(arr[3], c.rect_pos_, c.tex_);
        all_figure = { b00, b01, b02, b03 };
    }
    std::vector<vertex> one_fig_trianleses()
    {
        std::vector<vertex> vec_tr;
        std::vector<block>  fig_block;

        for (int i = 0; i < 4; i++)
        {
            cell_pos[i] *= cell_size;
            block b(cell_pos[i], cell_.rect_pos_, cell_.tex_);

            std::vector<vertex> res = b.build_block();
            std::copy_n(begin(res), 6, std::back_inserter(vec_tr));
        }

        return vec_tr;
    }
    block             cell_;         /// one block of figure
    vec2              fig_centr_pos; /// center of all figure
    std::vector<vec2> cell_pos;      /// position of cell for figure
    enum class direction
    {
        vertical,
        horisontal
    };
};

struct field
{
    field() = default;
    field(rect r, block c, texture* t)
        : rect_(r)
        , cell_(c)
        , tex_(t)
    {
    }
    std::vector<vec2> left_border;
    std::vector<vec2> right_border;
    void              border_coord()
    {
        size_t size = static_cast<size_t>(rect_.size.x * rect_.size.y);
        std::vector<vec2> cells_coor(size);

        std::vector<block> field_builded;
        block              temp;
        for (size_t i = 0; i < size; i++)
        {

            temp.centr_pos_.x = i % 10 * cell_size + cell_size * 0.5f;
            temp.centr_pos_.y = i / 10 * cell_size + cell_size * 0.5f;
            /// transform to gl
            temp.centr_pos_ -= gl_width * 0.5f;
            field_builded.push_back(temp);
        }

        for (size_t i = 0; i < static_cast<size_t>(rect_.size.y); i++)
        {
            left_border.push_back(
                field_builded[i + i * static_cast<size_t>(rect_.size.x)]
                    .centr_pos_);
            right_border.push_back(
                field_builded[static_cast<size_t>(rect_.size.x) +
                              i * static_cast<size_t>(rect_.size.x)]
                    .centr_pos_);
        }
    }

    std::vector<block> fill_field()
    {
        size_t size = static_cast<size_t>(rect_.size.x * rect_.size.y);
        std::vector<vec2> cells_coor(size);

        std::vector<block> field_builded;
        block              temp;
        for (size_t i = 0; i < size; i++)
        {

            temp.centr_pos_.x = i % 10 * cell_size + cell_size * 0.5f;
            temp.centr_pos_.y = i / 10 * cell_size + cell_size * 0.5f;
            /// transform to gl
            temp.centr_pos_ -= gl_width * 0.5f;
            field_builded.push_back(temp);
        }
        return field_builded;
    }
    std::vector<vertex> field_vertexes()
    {
        {
            std::array<vertex, 4> quad;
            // you can change colore of texture with shift uv coord

            /// P - pos_ center - OpenGL (0,0)
            /// vertex 3 - texture (0,0)
            /// 0 left-up, clockwise
            /// vec2.pos

            quad[0].pos.x = rect_.pos.x - rect_.size.x * 0.5;
            quad[0].pos.y = rect_.pos.y + rect_.size.y * 0.5;
            quad[1].pos.x = rect_.pos.x + rect_.size.x * 0.5;
            quad[1].pos.y = rect_.pos.y + rect_.size.y * 0.5;
            quad[2].pos.x = rect_.pos.x + rect_.size.x * 0.5;
            quad[2].pos.y = rect_.pos.y - rect_.size.y * 0.5;
            quad[3].pos.x = rect_.pos.x - rect_.size.x * 0.5;
            quad[3].pos.y = rect_.pos.y - rect_.size.y * 0.5;
            /// vec2.uv, OpenGL texture lower left angle is (0, 0)
            /// coordinate
            quad[3].uv.x = rect_.pos.x;
            quad[3].uv.y = rect_.pos.y;
            quad[0].uv.x = quad[3].uv.x;
            quad[0].uv.y += rect_.size.y * 0.5;
            ;
            quad[1].uv.x += rect_.size.x;
            quad[1].uv.y += rect_.size.y * 0.5;
            quad[2].uv.x += rect_.size.x;
            quad[2].uv.y = quad[3].uv.y;

            std::vector<vertex> quad_tri = { quad[3], quad[1], quad[2],
                                             quad[0], quad[3], quad[1] };
            return quad_tri;
        }
    }

    rect     rect_;
    block    cell_;
    bool     is_empty = true;
    texture* tex_     = nullptr;
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

    texture* texture_figure = engine->create_texture("blocks1.png");

    if (nullptr == texture_figure)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }
    texture* texture_window = engine->create_texture("frame.png");
    if (nullptr == texture_window)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }

    // block
    vec2                f_pos{ 0.0, 0.0 };
    rect                r{ { 0, 0 }, { 1.f / 7.f, 1 } };
    block               f(f_pos, r, texture_figure);
    std::vector<vertex> t_res = f.build_block();

    vertex_buffer* one_quad_buff =
        engine->create_vertex_buffer(&t_res[0], t_res.size());

    // end_block

    // figure
    vec2              central_pos(0.0f, 0.0f);
    std::vector<vec2> res1;
    for (int i = n; i < n + 4; i++)
    {
        res1.push_back(figures_coord[i].pos);
    }

    // field
    rect  main_rect({ 0, 0 }, { static_cast<float>(field_width) * cell_size,
                               static_cast<float>(field_height) * cell_size });
    block temp;
    field main_field(main_rect, temp, nullptr);
    std::vector<block> game_field = main_field.fill_field();

    figure              fig_I(f, res1, central_pos);
    std::vector<block>  b = fig_I.all_figure;
    std::vector<vertex> res2;
    //    std::for_each(begin(game_field), std::end(game_field), [&](block
    //    bl) {
    //        if (bl.is_empty)
    //        {
    //            if (bl.centr_pos_ == b[0].centr_pos_)
    //                bl = b[0];
    //            else if (bl.centr_pos_ == b[1].centr_pos_)
    //                bl = b[1];
    //            else if (bl.centr_pos_ == b[2].centr_pos_)
    //                bl = b[2];
    //            else if (bl.centr_pos_ == b[3].centr_pos_)
    //                bl = b[3];
    //        }
    //    });
    std::vector<vertex> res{ fig_I.one_fig_trianleses() };
    vertex_buffer*      vert_buff =
        engine->create_vertex_buffer(&res[0], res.size());

    std::vector<vertex> window_vertex{ main_field.field_vertexes() };

    vertex_buffer* vert_buff_window =
        engine->create_vertex_buffer(&window_vertex[0], window_vertex.size());

    // end_figure

    // start field;

    //

    vec2        current_pos(0.0f, 0.0f);
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
            float time = engine->get_time_from_init();
            timer += time;
            if (timer >= dt)
            {
                current_pos.y -= cell_size * 0.2;
                timer = 0.f;
            }
            switch (game_event.key)
            {
                case keys::exit:
                    continue_loop = false;
                    break;
                case keys::right:
                    if (engine->is_key_down(keys::right))
                    {
                        std::cout << "keys::right" << std::endl;
                        current_pos.x += cell_size;
                        float a = right_border;
                        if (!check_border(current_pos.x))
                        {
                            current_pos.x -= cell_size;
                        }
                        check = false;
                        break;

                        // game_event.is_down == false;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        current_pos.x -= cell_size;

                        if (!check_border(current_pos.x))
                        {
                            current_pos.x += cell_size;
                        }
                        check = false;
                        break;
                    }
                case keys::rotate:
                    if (engine->is_key_down(keys::rotate))
                    {
                        std::cout << "keys::rotate" << std::endl;
                        current_direction += pi * 0.5f;

                        check = false;
                        break;
                        // current_direction = pi / 2.f;
                    }
                default:
                    break;
            }
        }

        vec2   start_pos(0.0f, 1.0f - 1.5f * cell_size);
        matrix start         = matrix::move(start_pos);
        matrix move          = matrix::move(current_pos);
        matrix screen_aspect = matrix::scale(window_scale, 1.0f);
        matrix rot           = matrix::rotation(current_direction);
        matrix m             = rot * start * move * screen_aspect;

        // engine->render_tetris(*one_quad_buff, f.tex_);
        //  engine->render_tet(*one_quad_buff, f.tex_, matrix::identity());
        // engine->render_tetris(*vert_buff, fig_I.cell.tex_);
        engine->render_tet(*vert_buff_window, texture_window,
                           matrix::identity());
        engine->render_tet(*vert_buff, texture_figure, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

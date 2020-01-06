

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
// int figures[7][4]=
//    {
//        1,3,5,7, // I
//        2,4,5,7, // S
//        3,5,4,6, // Z
//        3,5,4,7, // T
//        2,3,5,7, // L
//        3,5,7,6, // J
//        2,3,4,5, // O
//    };

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
size_t field_height  = 20;
size_t field_width   = 10;
float  gl_height     = 2.f;
float  cell_size     = gl_height / field_height;
float  gl_width      = cell_size * field_width; // 2.f;
int    n             = 4;
float  left_border   = -gl_width * 0.5; // + cell_size * 0.5;
float  right_border  = gl_width * 0.5;  // - cell_size * 0.5;
float  bottom_border = gl_height * 0.5;
float  dt            = 1.f;
float  timer         = 0.f;

bool check_border(vec2& pos)
{
    if (pos.x < left_border + cell_size * 0.5 ||
        pos.x > right_border - cell_size * 0.5)
        return false;
    if (pos.y < -2 * bottom_border + cell_size * 3)
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
    block(rect pos, rect uv_pos, texture* tex)
        : xy_rect_(pos)
        , uv_rect_(uv_pos)
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
        quad[0].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5f;
        quad[0].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5f;
        quad[1].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5f;
        quad[1].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5f;

        quad[2].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5f;
        quad[2].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5f;
        quad[3].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5f;
        quad[3].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5f;
        /// vec2.uv, OpenGL texture lower left angle is (0, 0) coordinate
        quad[3].uv.x = uv_rect_.pos.x;
        quad[3].uv.y = uv_rect_.pos.y;
        quad[0].uv.x = quad[3].uv.x;
        quad[0].uv.y += uv_rect_.size.y;
        quad[1].uv.x += uv_rect_.size.x;
        quad[1].uv.y += uv_rect_.size.y;

        quad[2].uv.x += uv_rect_.size.x;
        quad[2].uv.y = quad[3].uv.y;

        std::vector<vertex> quad_tri = { quad[3], quad[1], quad[2],
                                         quad[0], quad[3], quad[1] };
        return quad_tri;
    }

    rect     xy_rect_;       /// centr of cell
    rect     uv_rect_;       /// position of texture
    texture* tex_ = nullptr; /// texture for cell
};

struct figure
{
    std::vector<block> all_figure;

    figure(block c, std::vector<vec2>& arr, vec2 pos)
        : cell_(c)
        , cell_pos{ arr }
        , fig_centr_pos(pos)
    {
        vec2  xy_rect_size = cell_.xy_rect_.size;
        block b00({ arr[0], xy_rect_size }, c.uv_rect_, c.tex_);
        block b01({ arr[1], xy_rect_size }, c.uv_rect_, c.tex_);
        block b02({ arr[2], xy_rect_size }, c.uv_rect_, c.tex_);
        block b03({ arr[3], xy_rect_size }, c.uv_rect_, c.tex_);
        all_figure = { b00, b01, b02, b03 };
    }

    std::vector<vertex> one_fig_trianleses()
    {
        std::vector<vertex> vec_tr;
        std::vector<block>  fig_block;

        for (int i = 0; i < 4; i++)
        {
            cell_pos[i] *= cell_size;
            block b({ cell_pos[i], cell_.xy_rect_.size }, cell_.uv_rect_,
                    cell_.tex_);

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
struct cell
{
    block cell_;
    bool  is_empty;
};

struct field
{
    field(size_t row, size_t col, float size)
        : col_(col)
        , row_(row)
        , cell_size_(size)
    {
        field_.resize(col * row);
        for (auto i = 0; i < col * row; i++)
        {
            field_[i].is_empty = true;
            float x = size * static_cast<float>(i % col) + size * 0.5f;
            float y = size * static_cast<float>(i / col) + size * 0.5f;
            //            x -= 1.f; // cell_size_ * col_ * 0.5f;
            //            y += 1.f; // cell_size_ * row_ * 0.5f;
            rect rect_xy({ x, y }, { size, size });
            rect rect_uv({ x, y }, { size, size });
            field_[i].cell_.xy_rect_ = rect_xy;
            field_[i].cell_.uv_rect_ = rect_uv;
        }
    }

    void set_figure(std::array<size_t, 4>& arr, block bl)
    {
        assert(arr.size() < field_.size());
        assert(nullptr != bl.tex_);
        for (auto v : arr)
        {
            size_t x = v % 2;
            size_t y = v / 2;

            field_[x + y * col_].cell_.tex_     = bl.tex_;
            field_[x + y * col_].cell_.uv_rect_ = bl.uv_rect_;
            field_[x + y * col_].is_empty       = false;
        }
    }
    vec2                centr_figure();
    std::vector<vertex> construct_field_vertexes()
    {
        std::vector<vertex> res;
        for (auto one_cell : field_)
        {
            if (!one_cell.is_empty)
            {
                std::vector<vertex> temp = one_cell.cell_.build_block();
                res.insert(end(res), begin(temp), end(temp));
            }
        }
        return res;
    }
    void build_figure(std::array<size_t, 4> figure, size_t pos, block& bl)
    {
        //        field_[pos]                       = bl;
        //        field_[pos + field_width - 1]     = bl;
        //        field_[pos + field_width]         = bl;
        //        field_[pos + 2 * field_width - 1] = bl;
    }
    bool cell_status(size_t n)
    {
        if (field_.at(n).is_empty)
            return true;
    }
    void              fill_cell(figure fig) {}
    size_t            col_;
    size_t            row_;
    float             cell_size_;
    std::vector<cell> field_;
};
void check(field& fil, const figure& fig) {}

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

    // block
    rect  f_pos{ { 0.0, 0.0 }, { cell_size, cell_size } };
    rect  r{ { 0, 0 }, { 1.f / 7.f, 1 } };
    block f(f_pos, r, texture_figure);

    rect     back_up_xy{ { 0.0, 0.0 }, { 2.f, 2.f } };
    rect     back_up_uv{ { 0, 0 }, { 1.f, 1.f } };
    texture* back_up_texture = engine->create_texture("frame.png");
    if (nullptr == back_up_texture)
    {
        std::cerr << "failed load texture\n";
        return EXIT_FAILURE;
    }
    block               back_up(back_up_xy, back_up_uv, back_up_texture);
    std::vector<vertex> back_up_vert = back_up.build_block();
    vertex_buffer*      back_up_vert_buff =
        engine->create_vertex_buffer(&back_up_vert[0], back_up_vert.size());
    // end_block

    // field
    field                 main_field(10, 10, cell_size);
    std::array<size_t, 4> fig_T{ 1, 2, 3, 5 };
    main_field.set_figure(fig_T, f);
    std::vector<vertex> temp_field_vertexes =
        main_field.construct_field_vertexes();
    assert(temp_field_vertexes.size() != 0);
    vertex_buffer* field_buff = engine->create_vertex_buffer(
        &temp_field_vertexes[0], temp_field_vertexes.size());

    // figure
    vec2              central_pos(0.0f, 0.0f);
    std::vector<vec2> res1;
    for (int i = n; i < n + 4; i++)
    {
        res1.push_back(figures_coord[i].pos);
    }

    figure              fig_I(f, res1, central_pos);
    std::vector<block>  b = fig_I.all_figure;
    std::vector<vertex> res{ fig_I.one_fig_trianleses() };
    vertex_buffer*      vert_buff =
        engine->create_vertex_buffer(&res[0], res.size());

    // end_figure

    vec2        current_pos(0.0f, 0.0f);
    const float pi = 3.1415926f;
    float       current_direction{ 0.f };
    float       current_scale{ 1.f };

    bool continue_loop = true;
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
                        current_pos.x += cell_size;
                        float a = right_border;
                        if (!check_border(current_pos))
                        {
                            current_pos.x -= cell_size;
                        }
                        break;

                        // game_event.is_down == false;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        current_pos.x -= cell_size;

                        if (!check_border(current_pos))
                        {
                            current_pos.x += cell_size;
                        }
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

        vec2   start_pos(0.0f, 1.0f - 0.5);
        matrix start         = matrix::move(start_pos);
        matrix move          = matrix::move(current_pos);
        matrix screen_aspect = matrix::scale(window_scale, 1.0f);
        matrix rot           = matrix::rotation(current_direction);
        matrix m             = rot * start * move * screen_aspect;

        //  engine->render_tet(*one_quad_buff, f.tex_, matrix::identity());
        // engine->render_tetris(*vert_buff, fig_I.cell.tex_);

        //        engine->render_tet(*back_up_buff, back_up_texture,
        //                           matrix::identity() *
        //                               matrix::scale(window_scale * 20, 20
        //                               * 1.0f));
        //        engine->render_tet(*back_up_vert_buff, back_up_texture,
        //                           screen_aspect); // back_ups
        //        engine->render_tet(*field_buff, texture_figure,
        //                           rot * matrix::move(current_pos) *
        //                               matrix::scale(window_scale, 1.0f));
        // engine->render_tetris(*field_buff, texture_figure, );
        engine->render_tet(*vert_buff, texture_figure, m);

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

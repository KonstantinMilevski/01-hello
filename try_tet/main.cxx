

#include "engine.hxx"
#include "texture_gl_es20.hxx"

#include <algorithm>
#include <array>
#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
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
            float x = size * static_cast<float>(i % row_) + size * 0.5f;
            float y = size * static_cast<float>(i / row_) + size * 0.5f;
            //            x -= 1.f; // cell_size_ * col_ * 0.5f;
            //            y += 1.f; // cell_size_ * row_ * 0.5f;
            rect rect_xy({ x, y }, { size, size });
            rect rect_uv({ x, y }, { size, size });
            field_[i].cell_.xy_rect_ = rect_xy;
            field_[i].cell_.uv_rect_ = rect_uv;
        }
    }
    vec2 get_position(size_t n) { return field_.at(n).cell_.xy_rect_.pos; }
    // size_t get_vector_pos() {}
    void set_position(block& bl, size_t n)
    {
        assert(n >= 0 && n < col_ * row_);
        vec2 pos                        = field_.at(n).cell_.xy_rect_.pos;
        field_.at(n).cell_              = bl;
        field_.at(n).cell_.xy_rect_.pos = pos;
    }

    void set_one_cell(block bl)
    {
        assert(nullptr != bl.tex_);
        float x = bl.xy_rect_.pos.x;
        float y = bl.xy_rect_.pos.y;

        field_[x + y * col_].cell_.tex_     = bl.tex_;
        field_[x + y * col_].cell_.uv_rect_ = bl.uv_rect_;
        // field_[x + y * col_].is_empty       = false;
    }
    std::vector<vertex> construct_one_moving_cell()
    {
        std::vector<vertex> res;
        for (auto cell : field_)
        {
            if (cell.is_empty && cell.cell_.tex_ != nullptr)
            {
                std::vector<vertex> temp = cell.cell_.build_block();
                res.insert(end(res), begin(temp), end(temp));
            }
        }
        assert(res.size() != 0);
        return res;
    }
    bool change_pos(size_t pos_old, size_t pos_new, bool flag) // left flag
    {
        if (pos_new >= 0 && pos_new < col_ * row_)
        {
            float d_pos_x_right =
                field_.at(pos_old).cell_.xy_rect_.pos.x -
                field_.at(row_ * col_ - 1).cell_.xy_rect_.pos.x;
            float d_pos_x_left = field_.at(pos_old).cell_.xy_rect_.pos.x -
                                 field_.at(0).cell_.xy_rect_.pos.x;
            if (d_pos_x_left != 0 && flag)

            {
                if (field_.at(pos_old).cell_.tex_ != nullptr)
                {
                    field_.at(pos_new).cell_.tex_ =
                        field_.at(pos_old).cell_.tex_;
                    field_.at(pos_new).cell_.uv_rect_ =
                        field_.at(pos_old).cell_.uv_rect_;
                    field_.at(pos_old).cell_.tex_ = nullptr;
                    return true;
                }
                else
                {
                    std::cout << "Cell not empty" << std::endl;
                    return false;
                }
            }
            else if (d_pos_x_right != 0 && !flag)
            {
                if (field_.at(pos_old).cell_.tex_ != nullptr)
                {
                    field_.at(pos_new).cell_.tex_ =
                        field_.at(pos_old).cell_.tex_;
                    field_.at(pos_new).cell_.uv_rect_ =
                        field_.at(pos_old).cell_.uv_rect_;
                    field_.at(pos_old).cell_.tex_ = nullptr;
                    return true;
                }
                else
                {
                    std::cout << "Cell not empty" << std::endl;
                    return false;
                }
            }

            return false;
        }
    }
    bool moving(int n) // 1 -left, 2-right
    {
        int k;
        for (int i = 0; i < col_ * row_; i++)
        {
            if (field_[i].is_empty && field_[i].cell_.tex_ != nullptr)
            {
                k = i;
            }
        }
        //        auto it = std::find(field_.begin(), field_.end(),
        //                            fig); //(begin(field_), end(field_),
        //                            fig);
        if (k >= 0 && k < col_ * row_)
        {
            if (1 == n)
            {
                if (field_[k - 1].is_empty && (k - 1) % col_ && (k - 1) >= 0)
                    return true;
            }
        }
        return false;
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
    /// field srart
    rect bloc_pos({ 0.f, 0.f }, { cell_size, cell_size });
    rect bloc_text({ 0.f, 0.f }, { 1.f / 7, 1.f });

    block first_bl(bloc_pos, bloc_text, text_main_bar);
    field main_field(10, 2, cell_size);
    int   pos = 1;
    // main_field.set_one_cell(first_bl);
    main_field.set_position(first_bl, pos);
    std::vector<vertex> one_block_vert;
    vertex_buffer*      moving_vertex_buffer;

    /// field end

    // vec2        current_pos(main_field.get_position(pos));
    const float pi = 3.1415926f;
    float       current_direction{ 0.f };

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

                        if (main_field.change_pos(pos, pos + 1, false))
                        {
                            // if (main_field.moving(1))
                            pos++;
                        }
                        break;

                        // game_event.is_down == false;
                    }
                case keys::left:
                    if (engine->is_key_down(keys::left))
                    {
                        std::cout << "keys::left" << std::endl;
                        if (main_field.change_pos(pos, pos - 1, true))
                        {
                            // if (main_field.moving(1))
                            pos--;
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
        one_block_vert       = main_field.construct_one_moving_cell();
        moving_vertex_buffer = engine->create_vertex_buffer(
            &one_block_vert[0], one_block_vert.size());

        vec2   start_pos(main_field.get_position(4)); //(0.0f, 1.0f - 0.5);
        matrix start = matrix::move(start_pos);
        // matrix move          = matrix::move(current_pos);
        matrix screen_aspect = matrix::scale(window_scale, 1.0f);
        matrix rot           = matrix::rotation(current_direction);
        matrix m             = rot * start /** move */ * screen_aspect;

        engine->render_tet(*moving_vertex_buffer, text_main_bar,
                           matrix::scale(window_scale, 1.0f));

        engine->swap_buffer();
    }

    engine->uninitialize();
    return 0;
}

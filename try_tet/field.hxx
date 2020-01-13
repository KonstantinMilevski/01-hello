#pragma once
#include "engine.hxx"
#include <array>

/// global

static float window_scale = static_cast<float>(screen_height) / screen_width;
/// field 10x25 GL from -0.5 to 0.5 gl_width     =1.f
/// field 10x25 GL from -0.1 to 0.1 gl_width     =2.f
constexpr size_t field_height = 20.f;
constexpr size_t field_width  = 10.f;
constexpr size_t cell_size    = 10.f;

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

    std::vector<vertex> build_block();
    void                set_position(vec2 new_pos);

    rect     xy_rect_;       /// centr of cell
    rect     uv_rect_;       /// position of texture
    texture* tex_ = nullptr; /// texture for cell
};

struct cell
{
    block cell_;
    bool  is_empty = true;
};
struct figure
{
    figure() = default;
    std::array<vec2, 4> figure_XY_coord;

    figure(std::array<size_t, 4>& coord);
    figure operator=(std::array<size_t, 4>& coord);
    void   figure_change_position(const size_t& pos);
    void   figure_rotate();

    std::array<size_t, 4> coord_;
};

struct field
{
    std::array<size_t, 4> cur_fig;

    field(size_t col, size_t row);
    void                refill_field();
    void                set_block_on_field(block& bl, const size_t& pos);
    void                set_figure(figure& fig, block& bl);
    std::vector<vertex> occupied_cells();
    void                clear_position(const figure& fig);
    void                clear_field();
    vec2                return_cell_pos(size_t n);
    bool                check_field_border(const figure& fig);
    bool                check_empty_cell(const figure& fig);
    bool check_figure_horizont(const figure& old, const figure& next);
    bool check_full_line(std::vector<cell>::iterator line);
    void check_field();

    size_t            col_;
    size_t            row_;
    std::vector<cell> field_;
};

// float gl_height = 2.f;

// float gl_width      = cell_size * field_width; // 2.f;
// int   n             = 4;
// float left_border   = -gl_width * 0.5; // + cell_size * 0.5;
// float right_border  = gl_width * 0.5;  // - cell_size * 0.5;
// float bottom_border = gl_height * 0.5;
// float dt            = 1.f;
// float timer         = 0.f;

// tri2 transform_pixel_coord_to_GL(size_t tex_w, size_t tex_h, tri2& t)
//{
//    tri2 n;
//    n.v[0].pos.x = t.v[0].pos.x * 2 / width - 1.0f;
//    n.v[0].pos.y = t.v[0].pos.y * 2 / heigh - 1.0f;
//    n.v[1].pos.x = t.v[1].pos.x * 2 / width - 1.0f;
//    n.v[1].pos.y = t.v[1].pos.y * 2 / heigh - 1.0f;
//    n.v[2].pos.x = t.v[2].pos.x * 2 / width - 1.0f;
//    n.v[2].pos.y = t.v[2].pos.y * 2 / heigh - 1.0f;

//    n.v[0].uv.x = t.v[0].uv.x * 2 / width - 1.0f;
//    n.v[0].uv.y = t.v[0].uv.y * 2 / heigh - 1.0f;
//    n.v[1].uv.x = t.v[1].uv.x * 2 / width - 1.0f;
//    n.v[1].uv.y = t.v[1].uv.y * 2 / heigh - 1.0f;
//    n.v[2].uv.x = t.v[2].uv.x * 2 / width - 1.0f;
//    n.v[2].uv.y = t.v[2].uv.y * 2 / heigh - 1.0f;
//    return n;
//}

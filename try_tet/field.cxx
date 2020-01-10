#include "field.hxx"

#include <algorithm>
#include <array>
#include <cassert>

std::vector<vertex> block::build_block()
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
    quad[0].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5;
    quad[0].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5;
    quad[1].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5;
    quad[1].pos.y = xy_rect_.pos.y + xy_rect_.size.y * 0.5;

    quad[2].pos.x = xy_rect_.pos.x + xy_rect_.size.x * 0.5;
    quad[2].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5;
    quad[3].pos.x = xy_rect_.pos.x - xy_rect_.size.x * 0.5;
    quad[3].pos.y = xy_rect_.pos.y - xy_rect_.size.y * 0.5;

    /// vec2.uv, OpenGL texture lower left angle is (0, 0) coordinate
    quad[3].uv.x = uv_rect_.pos.x;
    quad[3].uv.y = uv_rect_.pos.y;
    quad[0].uv.x = quad[3].uv.x;
    quad[0].uv.y = quad[3].uv.y + uv_rect_.size.y;
    quad[1].uv.x = quad[3].uv.x + uv_rect_.size.x;
    quad[1].uv.y = quad[3].uv.y + uv_rect_.size.y;

    quad[2].uv.x = quad[3].uv.x + uv_rect_.size.x;
    quad[2].uv.y = quad[3].uv.y;

    std::vector<vertex> quad_tri = { quad[1], quad[2], quad[3],
                                     quad[0], quad[1], quad[3] };
    return quad_tri;
}

void block::set_position(vec2 new_pos)
{
    xy_rect_.pos = new_pos;
    uv_rect_.pos = new_pos;
}
///////////////////////////////
field::field(size_t row, size_t col)
    : col_(col)
    , row_(row)
{
    {
        field_.resize(col * row);
        for (auto i = 0; i < col * row; i++)
        {
            field_.at(i).is_empty = true;
            float x               = i % row_ * cell_size + 0.5 * cell_size;
            float y               = i / row_ * cell_size + 0.5 * cell_size;
            rect  rect_xy({ x, y }, { cell_size, cell_size });
            rect  rect_uv({ x, y }, { cell_size, cell_size });
            field_.at(i).cell_.xy_rect_ = rect_xy;
            field_.at(i).cell_.uv_rect_ = rect_uv;
        }
    }
}

void field::set_block_on_field(block& bl, const size_t& pos)
{
    assert(pos >= 0);
    assert(pos < col_ * row_);
    vec2 cell_pos = this->return_cell_pos(pos);
    bl.set_position(cell_pos);
    field_.at(pos).cell_.uv_rect_.size = bl.uv_rect_.size;
    field_.at(pos).is_empty            = false;
}

bool field::set_figure(figure& figur, block& bl)
{
    if (field_.at(figur.coord_.at(0)).is_empty == true &&
        field_.at(figur.coord_.at(1)).is_empty == true &&
        field_.at(figur.coord_.at(2)).is_empty == true &&
        field_.at(figur.coord_.at(3)).is_empty == true)
    {
        for (size_t i = 0; i < 4; i++)
        {
            this->set_block_on_field(bl, figur.coord_.at(i));
        }
        return true;
    }
    return false;
}

std::vector<vertex> field::occupied_cells()
{
    std::vector<vertex> occupied_pos;
    for (auto var : field_)
    {
        if (!var.is_empty)
        {
            std::vector<vertex> temp_vertexes = var.cell_.build_block();
            occupied_pos.insert(end(occupied_pos), begin(temp_vertexes),
                                end(temp_vertexes));
        }
    }
    return occupied_pos;
}

void field::clear_position(const figure& fig)
{
    for (auto cell : fig.coord_)
    {
        field_.at(cell).is_empty = true;
    }
}

bool field::figure_in_field(block& bl, std::array<size_t, 4>& fig)
{
    if (fig.at(0) >= 0 && fig.at(0) < row_ * col_ && fig.at(1) >= 0 &&
        fig.at(1) < row_ * col_ && fig.at(2) >= 0 && fig.at(2) < row_ * col_ &&
        fig.at(3) >= 0 && fig.at(3) < row_ * col_)
    {
        for (auto f : fig)
        {
            this->set_block_on_field(bl, f);
        }
        return true;
    }
    return false;
}

void field::clear_all_field()
{
    for (auto& cells : field_)
    {
        cells.is_empty = true;
    }
}

vec2 field::return_cell_pos(size_t n)
{
    return field_.at(n).cell_.xy_rect_.pos;
}

size_t field::return_vector_pos(const vec2& pos)
{

    return pos.y * row_ + pos.x;
}

bool field::check_field_border(const figure& fig)
{
    for (auto i = 0; i < 4; i++)
    {
        if (fig.coord_.at(i) >= row_ * col_ || fig.coord_.at(i) < 0)
            return false;
    }
    return true;
}

bool field::check_empty_cell(const figure& fig)
{
    for (auto i = 0; i < 4; i++)
    {
        if (field_.at(fig.coord_.at(0)).is_empty == false)
            return false;
    }
    return true;
}
bool field::check_figure_horiszont(const figure& old, const figure& next)
{
    for (auto i = 0; i < 4; i++)
    {
        if (old.coord_.at(i) / 10 != next.coord_.at(i) / 10)
            return false;
    }
    return true;
}

figure::figure(std::array<size_t, 4>& coord)

    : coord_{ coord }
{
    for (size_t i = 0; i < 4; i++)
    {
        figure_XY_coord.at(i).x =
            coord_.at(i) % 10; //* cell_size + 0.5 * cell_size;
        figure_XY_coord.at(i).y =
            coord_.at(i) / 10; // * cell_size + 0.5 * cell_size;
    }
}

void figure::figure_change_position(const size_t& pos)
{

    for (auto& block : coord_)
    {
        block += pos;
    }
}

bool figure::figure_horiszontal_move(const size_t& pos)
{
    // return size 10-column, 200-col*row

    std::array<size_t, 4> old_figure = coord_;

    if (coord_.at(0) + pos >= 0 && coord_.at(0) + pos < 200 &&
        coord_.at(1) + pos >= 0 && coord_.at(1) + pos < 200 &&
        coord_.at(2) + pos >= 0 && coord_.at(2) + pos < 200 &&
        coord_.at(3) + pos >= 0 && coord_.at(3) + pos < 200 &&
        old_figure.at(0) / 10 == (coord_.at(0) + pos) / 10 &&
        old_figure.at(1) / 10 == (coord_.at(1) + pos) / 10 &&
        old_figure.at(2) / 10 == (coord_.at(2) + pos) / 10 &&
        old_figure.at(3) / 10 == (coord_.at(3) + pos) / 10)
    {
        for (auto& block : coord_)
        {
            block += pos;
        }
        return true;
    }
    else
    {
        coord_ = old_figure;
        return false;
    }
}

void figure::figure_rotate()
{
    std::array<vec2, 4> coord_XY;
    for (auto i = 0; i < 4; i++)
    {
        float x          = coord_.at(i) % 10;
        float y          = coord_.at(i) / 10;
        coord_XY.at(i).x = x;
        coord_XY.at(i).y = y;
    }
    vec2 centr = coord_XY.at(1);
    vec2 new_pos;
    bool move_left  = false;
    bool move_right = false;
    for (auto i = 0; i < 4; i++)
    {
        new_pos.x = centr.x - (coord_XY.at(i).y - centr.y);
        new_pos.y = centr.y + (coord_XY.at(i).x - centr.x);
        if (new_pos.x < 0)
            move_right = true;
        if (new_pos.x > 9)
            move_left = true;
        coord_.at(i) = new_pos.x + new_pos.y * 10;
    }
    if (move_right)
    {
        for (auto i = 0; i < 4; i++)
        {
            coord_.at(i)++;
        }
    }
    if (move_left)
        for (auto i = 0; i < 4; i++)
        {
            coord_.at(i)--;
        }
}

bool figure::figure_move_down()
{
    int                   pos        = -10;
    std::array<size_t, 4> old_figure = coord_;

    if (coord_.at(0) + pos >= 0 && coord_.at(0) + pos < 200 &&
        coord_.at(1) + pos >= 0 && coord_.at(1) + pos < 200 &&
        coord_.at(2) + pos >= 0 && coord_.at(2) + pos < 200 &&
        coord_.at(3) + pos >= 0 && coord_.at(3) + pos < 200)
    {

        for (auto& block : coord_)
        {
            block += pos;
        }
        return true;
    }
    else
    {
        coord_ = old_figure;
        return false;
    }
}
